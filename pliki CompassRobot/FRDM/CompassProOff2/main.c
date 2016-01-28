#include "MKL46Z4.h"         
#include "slcd.h"   
#include "leds.h"     
#include "compass.h"   
#include "mag.h"  
#include "mag2.h"  
#include "buttons.h"
#include "motorDriver.h"
#include "blue.h"


int main(void){
	
	ledsInitialize();
	slcdInitialize();
	buttonsInitialize();
	blueInit();
	eCompass_init();
	motorDriverInit();
	selectCompass(0);
	
	while(1){
	}	
}
