/**
@mainpage MAG3110
@brief Biblioteka obslugi kompasu MAG3110
@details Pobierz i dodaj do stwojego projektu pliki:

compass.c ,compass.h funckcje wysokiego poziomu obslugi kompasu

mag.c, mag.h  funkcje niskiego poziomu obslugi kompasu

i2c.c, i2c.h  funkcje obslugi komunikacji i2c
		
W pliku main.c dodaj  #include "compass.h"  

	Aby zainicjalizowac dzialanie kompasu uzyj funkcji eCompass_init() . Przy pierwszym uzyciu kompas nie jest skalibrowany. Aby go skalibrowac nalezy wybrac funkcje startCalibration()  i obrocic kompas o co najmniej 360 stopni  plytka powinna lezec plasko, po czym wywylac funckje stopCalibration()  .Im wolniej obracasz kopmpasem tym dajesz szanse na dokladniejsza kalibracje. Pamietaj, aby podczas kalibracji kompass nie byl zaklocany zadnymi magnesami lub metalami.  Dane kalibracji zostaja zapamietane (poza wspolczynnikiem AR) i nie jest wymagane przeprowadzanie kolejnych kalibracji. Kat obrotu kompasu odczytujemy przy uzyciu funckji eCompass(). Dane zwracane sa jako kat 0 do 360 stopni.
*/


/** \file compass.h
	\brief Obsluga kompasu wysokiego poziomu
*/


#include "compass.h"
#include "math.h"
#include "leds.h"

#define DegToRad 0.01745329251994F
#define RadToDeg 57.2957795130823F	


float Psi; 
signed short maxX=-20000;
signed short minX=20000;
signed short maxY=-20000;
signed short minY=20000;
float AR=1;

uint8_t avarag=10;

/** \brief Inicjalizacja kompasu */
void eCompass_init(void)
{
	mag_init();
	//accel_init();
	//mag1_init();
}



/** \brief Wypisanie kata obrotu kompasu
		\retuen obrocenie kompasu */
int eCompass (void)
{
	int i=0;
	float sumaX=0;
	float sumaY=0;
	
	for(i=0;i<avarag;i++){
		int iBx =0, iBy =0;
		iBy = iMagReadY();
		iBx = iMagReadX();
		sumaX += (float) iBx;
		sumaY += (float) iBy*AR;
	}
		
	Psi = atan2(-sumaY, sumaX) * RadToDeg;
	Psi += 180;
	if(Psi<0)Psi += 360;
	if(Psi>360)Psi -= 360;
	return Psi;
}



/** \brief Funkcja rozpoczynajaca kalibracje */
void startCalibration(){

	maxX=-20000;
	minX=20000;
	maxY=-20000;
	minY=20000;
	
	 hal_dev_mag3110_write_reg(MAG_OFF_X_MSB,0); 
	 hal_dev_mag3110_write_reg(MAG_OFF_X_LSB,0); 
	 hal_dev_mag3110_write_reg(MAG_OFF_Y_MSB,0); 
	 hal_dev_mag3110_write_reg(MAG_OFF_Y_LSB,0); 
	
	SIM->SCGC6 |=  (SIM_SCGC6_PIT_MASK);     
	PIT->CHANNEL[0].LDVAL = 0x33456;
	NVIC_ClearPendingIRQ(22);				
	NVIC_EnableIRQ(22);	
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TIE_MASK;
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	PIT->MCR = 0x00;
}



/** \brief Funkcja konczaca kalibracje */
void stopCalibration(void){
	signed short dataX=0;
	signed short dataY=0;
	dataX = maxX+minX;
	dataY = maxY+minY;
	
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;
	PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
	NVIC_DisableIRQ(22);	
	SIM->SCGC6 &=  ~(SIM_SCGC6_PIT_MASK);   
	
	hal_dev_mag3110_write_reg(MAG_OFF_X_MSB,(dataX)>>8); 
	hal_dev_mag3110_write_reg(MAG_OFF_X_LSB,dataX); 
	hal_dev_mag3110_write_reg(MAG_OFF_Y_MSB,(dataY)>>8); 
	hal_dev_mag3110_write_reg(MAG_OFF_Y_LSB,dataY); 
	
	AR = fabs(maxX-minX)/fabs(maxY-minY);
}




int ava3=0;
int sumaX = 0;
int sumaY = 0;

/** \brief Obsluga przerwan podczas kalibracji kompasu */
void PIT_IRQHandler(void){
	
	if((PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) !=0){

	sumaX+=(iMagReadX());
	sumaY+=(iMagReadY());
	ava3++;
	if(ava3==4){
		if(maxX<(sumaX>>2))maxX=(sumaX>>2);
		if(minX>(sumaX>>2))minX=(sumaX>>2);
		if(maxY<(sumaY>>2))maxY=(sumaY>>2);
		if(minY>(sumaY>>2))minY=(sumaY>>2);
		sumaX=0;
		sumaY=0;
		ava3=0;
	}
	
	ledGreenOn();
	PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
}}



/** \brief Funkcja okreslajaca usrednianie wynikow
		\param num ilosc pomiarow aby dac jeden wynik */
void setAvaraging(uint8_t num){
	avarag=num;
}