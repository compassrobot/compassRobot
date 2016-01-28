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
#include "blue.h"
#include "slcd.h"

#define DegToRad 0.01745329251994F
#define RadToDeg 57.2957795130823F	


float Psi; 
signed short maxX=-20000;
signed short minX=20000;
signed short maxY=-20000;
signed short minY=20000;

signed short maxX2=-20000;
signed short minX2=20000;
signed short maxY2=-20000;
signed short minY2=20000;
float AR0=1;
float AR1=1;
float AR2=1;

int compassSelect=2;


uint8_t avarag=1;

/** \brief Inicjalizacja kompasu */
void eCompass_init(void)
{
	mag_init();
	//accel_init();
	mag2_init();
	
	__enable_irq();
	SIM->SCGC6 |=  (SIM_SCGC6_PIT_MASK);     
	NVIC_ClearPendingIRQ(22);				
	NVIC_SetPriority(22,1);
	NVIC_EnableIRQ(22);	
	PIT->MCR = 0x00;
	
	PIT->CHANNEL[1].LDVAL = 0x310000;
	PIT->CHANNEL[1].TCTRL = PIT_TCTRL_TIE_MASK;
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;
	PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;
}



/** \brief Wypisanie kata obrotu kompasu
		\retuen obrocenie kompasu */
int eCompass (void)
{
	int i=0;
	float sumaX=0;
	float sumaY=0;
	float sumaX2=0;
	float sumaY2=0;
	
	
	for(i=0;i<avarag;i++){
		int iBx =0, iBy =0;

		switch (compassSelect){
			case 0:
				iBy = iMagReadY()+iMag2ReadY();
				iBx = -iMagReadX()+ iMag2ReadX();
				sumaX += (float) iBx;
				sumaY += (float) iBy*AR0;
				break;
			case 1:
				iBy = iMagReadY();
				iBx = -iMagReadX();
				sumaX += (float) iBx;
				sumaY += (float) iBy*AR1;
				break;
			case 2:
				iBy = iMag2ReadY();
				iBx = iMag2ReadX();
				sumaX += (float) iBx;
				sumaY += (float) iBy*AR2;
				break;
			default:
				iBy = iMagReadY();
				iBx = -iMagReadX();
				sumaX += (float) iBx;
				sumaY += (float) iBy*AR1;
			
				iBy = iMag2ReadY();
				iBx = iMag2ReadX();
				sumaX2 += (float) iBx;
				sumaY2 += (float) iBy*AR2;
				break;
		}
	}
		
	Psi = atan2(-sumaY, sumaX) * RadToDeg;
	
	if(compassSelect==3){Psi = (atan2(-sumaY, sumaX)+atan2(-sumaY2, sumaX2)) * RadToDeg;Psi/=2;}
	
	Psi += 180;
	if(Psi<0)Psi += 360;
	if(Psi>360)Psi -= 360;
	return Psi;
}

int ava3=0;
int sumaX = 0;
int sumaY = 0;

int sumaX2 = 0;
int sumaY2 = 0;

/** \brief Funkcja rozpoczynajaca kalibracje */
void startCalibration(){

	maxX=-20000;
	minX=20000;
	maxY=-20000;
	minY=20000;
	
	
	maxX2=-20000;
	minX2=20000;
	maxY2=-20000;
	minY2=20000;
	
	
	ava3=0;
	sumaX = 0;
	sumaY = 0;
	
	sumaX2 = 0;
	sumaY2 = 0;
	
	
	 hal_dev_mag3110_write_reg(MAG_OFF_X_MSB,0); 
	 hal_dev_mag3110_write_reg(MAG_OFF_X_LSB,0); 
	 hal_dev_mag3110_write_reg(MAG_OFF_Y_MSB,0); 
	 hal_dev_mag3110_write_reg(MAG_OFF_Y_LSB,0); 
	
	 hal_dev_mag2_write_reg(0x16,0); 
	 hal_dev_mag2_write_reg(0x17,0); 
	 hal_dev_mag2_write_reg(0x18,0); 
	 hal_dev_mag2_write_reg(0x19,0); 
	
	
	
	
	PIT->CHANNEL[0].LDVAL = 0x33456;
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TIE_MASK;
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
	
	
}



/** \brief Funkcja konczaca kalibracje */
void stopCalibration(void){
	signed short dataX=0;
	signed short dataY=0;
	dataX = maxX+minX;
	dataY = maxY+minY;
	
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;
	PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
	//NVIC_DisableIRQ(22);	
	//SIM->SCGC6 &=  ~(SIM_SCGC6_PIT_MASK);   
	
	hal_dev_mag3110_write_reg(MAG_OFF_X_MSB,(dataX)>>8); 
	hal_dev_mag3110_write_reg(MAG_OFF_X_LSB,dataX); 
	hal_dev_mag3110_write_reg(MAG_OFF_Y_MSB,(dataY)>>8); 
	hal_dev_mag3110_write_reg(MAG_OFF_Y_LSB,dataY); 
	
	//drugi kompass
	dataX = ((maxX2+minX2)/2);
	dataY = ((maxY2+minY2)/2);
	hal_dev_mag2_write_reg(0x17,(dataX)>>8); 
	hal_dev_mag2_write_reg(0x16,dataX); 
	hal_dev_mag2_write_reg(0x19,(dataY)>>8); 
	hal_dev_mag2_write_reg(0x18,dataY); 
	
	
	AR0 = (fabs(maxX2-minX2)+fabs(maxX-minX))/(fabs(maxY2-minY2)+fabs(maxY-minY));
	AR1 = (fabs(maxX-minX))/(fabs(maxY-minY));
	AR2 = (fabs(maxX2-minX2))/(fabs(maxY2-minY2));
	
	ledsOff();
}



/** \brief Obsluga przerwan podczas kalibracji kompasu */
void PIT_IRQHandler(void){
	
	if((PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) !=0){
	ledGreenOn();
	sumaX+=(iMagReadX());
	sumaY+=(iMagReadY());
		
	sumaX2+=(iMag2ReadX());
	sumaY2+=(iMag2ReadY());
		
	ava3++;
	if(ava3==4){
		if(maxX<(sumaX>>2))maxX=(sumaX>>2);
		if(minX>(sumaX>>2))minX=(sumaX>>2);
		if(maxY<(sumaY>>2))maxY=(sumaY>>2);
		if(minY>(sumaY>>2))minY=(sumaY>>2);
		
		if(maxX2<(sumaX2>>2))maxX2=(sumaX2>>2);
		if(minX2>(sumaX2>>2))minX2=(sumaX2>>2);
		if(maxY2<(sumaY2>>2))maxY2=(sumaY2>>2);
		if(minY2>(sumaY2>>2))minY2=(sumaY2>>2);
		sumaX=0;
		sumaY=0;
		sumaX2=0;
		sumaY2=0;
		ava3=0;
	}
	
	ledGreenOn();
	PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
}

	if((PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) !=0){
				nadaj(eCompass());	
		PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;
	}


}



/** \brief Funkcja okreslajaca usrednianie wynikow
		\param num ilosc pomiarow aby dac jeden wynik, domyslnie to 0 */
void setAvaraging(uint8_t num){
	avarag=num;
}


/** \brief Funkcja wybor kompasu z ktorego korzystamy
		\param sc 0[domyslnie] -> dane z dwoch kompasow, 1 -> kompass na plytce(niezalecane), 2->kompass na robocie, 3->dwa kompasy, ale kat liczony jako srednia katow kompasow.  */
void selectCompass(int sc){
	compassSelect = sc;
}