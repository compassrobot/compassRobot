#include "MKL46Z4.h"         
#include "slcd.h"   
#include "leds.h"     
#include "compass.h"   
#include "mag.h"  
#include "mag2.h"  
#include "buttons.h"
#include "motorDriver.h"
#include "blue.h"

void waitS(void){
	int i=0;
	int j=0;
	for(i=0;i<0x7fff;i++)
			for(j=0;j<0xFf;j++);
}

void waitSS(void){
	int i=0;
	int j=0;
	for(i=0;i<0x1fff;i++)
			for(j=0;j<0xFf;j++);
}

void calibrate(void){
	startCalibration();
	ledGreenOn();
	turnLeft();
	waitS();
	waitS();
	waitS();
	waitS();
	waitS();
	waitS();
	waitS();
	waitS();
	waitS();
	waitS();
	waitS();
	waitS();
	driveStop();
	stopCalibration();
	ledsOff();
	slcdDisplay(0,2);
}

int main(void){
	
	uint16_t kat=0;
	
	ledsInitialize();
	slcdInitialize();
	buttonsInitialize();
	blueInit();
	eCompass_init();
	motorDriverInit();
		//calibrate();
	
	
	selectCompass(0);
	
	while(1){
	}
	
	/*driveForward(20);
	jedzProsto();
	
	while(1){

	}*/
	
	/*
	while(1){
		driveForward(40);
		waitS();
		turnLeftC(90);
		while (ifReady()==0){}
		kat = eCompass();
		slcdDisplay(kat,2);
		waitS();
		turnLeftDir(90);
		while (ifReady()==0){}
		kat = eCompass();
		slcdDisplay(kat,2);
		waitS();
		turnLeftC(90);
		while (ifReady()==0){}
		kat = eCompass();
		slcdDisplay(kat,2);
		waitS();
		turnLeftC(90);
		while (ifReady()==0){}
		kat = eCompass();
		slcdDisplay(kat,2);
		}*/
		
			/*while(1){
				driveForward(0);
				selectCompass(1);
				turnLeftDir(40);
				while (ifReady()==0){}
				waitS();
				turnRightDir(110);
				while (ifReady()==0){}
				waitS();
				selectCompass(2);
				turnLeftDir(40);
				while (ifReady()==0){}
				waitS();
				turnRightDir(110);
				while (ifReady()==0){}
				waitS();
				selectCompass(3);
				turnLeftDir(40);
				while (ifReady()==0){}
				waitS();
				turnRightDir(110);
				while (ifReady()==0){}
				waitS();	
				}*/
				
			/*		while(1){
				driveForward(0);
				turnRightC(90);
				while (ifReady()==0){}
				waitS();
				turnRightC(90);
				while (ifReady()==0){}
				waitS();
				turnRightC(90);
				while (ifReady()==0){}
				waitS();
				turnRightC(90);
				while (ifReady()==0){}
				waitS();
				turnRightC(90);
				while (ifReady()==0){}
				waitS();
				turnRightC(90);
				while (ifReady()==0){}
				waitS();	
				}*/
	}
