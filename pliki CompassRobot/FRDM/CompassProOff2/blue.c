#include "blue.h"
#include "slcd.h"
#include "math.h"
#include "compass.h"
#include "motorDriver.h"


uint8_t funkjonalnosc=0x14;


void UART2_IRQHandler(void){
		int i=0;
		int j=0;
		uint8_t	temp = UART2->S1;			
		uint16_t	kat = UART2->D;	
		slcdDisplay(kat,2);

	if(funkjonalnosc!=0x51 && funkjonalnosc!=0x53){
	switch(kat){
		case 65:
			selectCompass(0);
			break;
		case 66:
			selectCompass(1);
			break;
		case 67:
			selectCompass(2);
			break;
		case 68:
			selectCompass(3);
			break;
		case 69:
			zmienPredkosc(0);
			break;
		case 70:
			zmienPredkosc(10);
			break;
		case 71:
			zmienPredkosc(20);
			break;
		case 72:
			zmienPredkosc(30);
			break;
		case 73:
			zmienPredkosc(40);
			break;
		case 74:
			zmienPredkosc(50);
			break;
		case 75:
			zmienPredkosc(60);
			break;
		case 76:
			zmienPredkosc(70);
			break;
		case 77:
			zmienPredkosc(80);
			break;
		case 78:
			zmienPredkosc(90);
			break;
		case 79:
			turnLeft();
			break;
		case 80:
			wyprostuj();
			break;
		case 81:
			turnRight();
			break;
		case 82:
			startCalibration();
			turnLeft();
			for(i=0;i<0xfFf;i++){
				for(j=0;j<0x44ff;j++);}
			driveStop();
			stopCalibration();
			break;
		case 83:
			NVIC_DisableIRQ(22);
			turnRightC(45);
			NVIC_EnableIRQ(22);
			break;
		case 84:
			NVIC_DisableIRQ(22);
			turnRightDir(180);
			NVIC_EnableIRQ(22);
			break;
		case 85:
			NVIC_DisableIRQ(22);
			turnLeftC(45);
			NVIC_EnableIRQ(22);
			break;
		case 87:
			funkjonalnosc=0x51;
			break;
		case 88:
			funkjonalnosc=0x53;
			break;
		default:
			break;
	}
}else{
	if(funkjonalnosc==0x53)kat+=256;
	funkjonalnosc=0x14;
	if(fabs(eCompass()-kat)>21 && fabs(eCompass()-kat)<339){
	NVIC_DisableIRQ(22);
	turnLeftDir(kat);
	NVIC_EnableIRQ(22);}
}
}



void blueInit(void){
	
	SIM->SCGC5|=SIM_SCGC5_PORTE_MASK;
	SIM->SCGC4|=SIM_SCGC4_UART2_MASK;
	
	PORTE->PCR[16] |= PORT_PCR_MUX(3); //UART2_TX
	PORTE->PCR[17] |= PORT_PCR_MUX(3); //UART2_RX
	
	 UART2->C2 &= ~UART_C2_TE_MASK; //zablokowac nadajnik i odbiornik, w rejestrze UART2->C2,
	 UART2->C2 &= ~UART_C2_RE_MASK;
	
	//UART2->BDH  //ustawic 13-bitowa wartosc dzielnika, bedacego zródlem zegara dla odbiornika inadajnika. (156)
	UART2->BDL = 0x9C;
	
	UART2->BDH &= ~UART_BDH_SBNS_MASK; //ustawic jeden bit stopu, w rejestrze UART2->BDH (bit SBNS),
	UART2->C1 &= ~UART_C1_PE_MASK; //? ustawic dlugosc danej na 8 bitów oraz brak sprzetowej obslugi sprawdzania parzystosci,w rejestrze UART2->C1 (bity M i PE),
  UART2->C1 &= ~UART_C1_M_MASK; 
	
	
	UART2->C2 |=UART_C2_RIE_MASK; //wlaczam przerwania od odbiornika
	
		__enable_irq();
	NVIC_ClearPendingIRQ(14);
	NVIC_EnableIRQ(14);
	NVIC_SetPriority(14,2);
	
	 UART2->C2 |= UART_C2_TE_MASK; //wlaczyc nadajnik i odbiornik, rejestrze UART2->C2.
	 UART2->C2 |= UART_C2_RE_MASK;

}

void nadaj(uint16_t dane){
	while((UART2->S1 & UART_S1_TDRE_MASK )== 0){}
			UART2->D = 0xff;	
	while((UART2->S1 & UART_S1_TDRE_MASK )== 0){}
			UART2->D = dane>>8;			
	while((UART2->S1 & UART_S1_TDRE_MASK )== 0){}
			UART2->D = dane;	
}