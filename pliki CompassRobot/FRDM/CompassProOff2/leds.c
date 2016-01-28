#include "MKL46Z4.h"                    //Device header
#include "leds.h"												//Declarations

const uint32_t red_mask= 1UL<<5;				//Red led is Port D bit 5
const uint32_t green_mask= 1UL<<29;			//Green led is Port C bit 5/


/*----------------------------------------------------------------------------
  Function that initializes LEDs
 *----------------------------------------------------------------------------*/
void ledsInitialize(void) {
volatile int delay;
	
//Initialize registers	
  SIM->SCGC5 |=  (SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK);      /* Enable Clock to Port D & E */ 
  PORTD->PCR[5] = PORT_PCR_MUX(1);                       /* Pin PTD5 is GPIO */
  PORTE->PCR[29] = PORT_PCR_MUX(1);                      /* Pin PTE29 is GPIO */
  
	FPTD->PSOR = red_mask	;          /* switch Red LED off */
	FPTE->PSOR = green_mask	;        /* switch Green LED off */
  FPTD->PDDR = red_mask	;          /* enable PTB18/19 as Output */
	FPTE->PDDR = green_mask	;        /* enable PTB18/19 as Output */
	

//Welcome sequence
	for(delay=0; delay<1200000; delay++);
	FPTD->PCOR = red_mask	;          /* switch Red LED on */
	FPTE->PCOR = green_mask	;        /* switch Green LED on */
	for(delay=0; delay<1200000; delay++);
	FPTD->PSOR = red_mask	;          /* switch Red LED off */
	FPTE->PSOR = green_mask	;        /* switch Green LED off */
	for(delay=0; delay<1200000; delay++);
	FPTD->PCOR = red_mask	;          /* switch Red LED on */
	FPTE->PCOR = green_mask	;        /* switch Green LED on */
	for(delay=0; delay<1200000; delay++);
	FPTD->PSOR = red_mask	;          /* switch Red LED off */
	FPTE->PSOR = green_mask	;        /* switch Green LED off */

}


/*----------------------------------------------------------------------------
  Function that turns on Red LED
 *----------------------------------------------------------------------------*/
void ledRedOn (void) {
	FPTD->PCOR=red_mask;          	/* switch Red LED on */
	FPTE->PSOR=green_mask;          /* switch Green LED off */
}

/*----------------------------------------------------------------------------
  Function that turns on Green LED
 *----------------------------------------------------------------------------*/
void ledGreenOn (void) {
	FPTE->PCOR=green_mask;       		/* switch Green LED on */
	FPTD->PSOR=red_mask;          	/* switch Red LED off  */
}

/*----------------------------------------------------------------------------
  Function that turns all LEDs off
 *----------------------------------------------------------------------------*/
void ledsOff (void) {
		//FPTD->PSOR=red_mask;          /* switch Red LED off  */
	  FPTE->PSOR=green_mask;        /* switch Green LED off  */
}

/*----------------------------------------------------------------------------
  Function that turns all LEDs on
 *----------------------------------------------------------------------------*/
void ledsOn (void) {
		FPTD->PCOR=red_mask;      	/* switch Red LED on  */
	  FPTE->PCOR=green_mask;     	/* switch Green LED on */
}
