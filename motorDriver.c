#include "motorDriver.h"
#include "compass.h"
#include "math.h"
//////////////////////////////////////// Global variables initialization ////////////////////////////////////////



// drive way and encoder turning functionality variables
uint32_t actualDistance = 0;
uint8_t flagLimitDistance = 0;
uint32_t limit = 0;

// compass turning functionality variables
uint8_t flagLimitAngle = 0;
uint32_t limitAngle = 0;
uint32_t offset = 0;

// distance counter functionality variables
uint32_t countersDistance = 0;
uint8_t flagCounting = 0;
uint8_t flagTurning = 0;

// turning speed
uint32_t turningSpeed = DEFAULT_TURNING_SPEED;

uint16_t azimuth = 0;

uint16_t tmp = 0;




//Piotr Klasa variable:
uint16_t ready = 1;
uint16_t avg_predkosc = 0;
uint16_t skret_speed = 23;

double pos_x=0;
double pos_y=0;

uint16_t kierunek = 0;
uint8_t prostaJazda = 0;
uint8_t poprawka = 0;
uint8_t poprawka_skret = 0;

int left=0;
int right=0;

//////////////////////////////////////// Functions ////////////////////////////////////////

void motorDriverInit(void)
{
	 // CLOCK_SETUP 1
	 //1 ... Multipurpose Clock Generator (MCG) in PLL Engaged External (PEE) mode
   //      Reference clock source for MCG module is an external crystal 8MHz
   //      Core clock = 48MHz, BusClock = 24MHz
	
	// connect the clock
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK
							| SIM_SCGC5_PORTC_MASK
							|	SIM_SCGC5_PORTD_MASK;
	
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	
	
	// choose signals for pins
	PORTA->PCR[6] |= PORT_PCR_MUX(3); // TPM0_CH3 - encoder
	PORTA->PCR[13] |= PORT_PCR_MUX(1); // PHASE - Left
	PORTC->PCR[9] |= PORT_PCR_MUX(1); // PHASE - Right
	PORTD->PCR[2] |= PORT_PCR_MUX(4); // TPM0_CH2 - PWM - Right
	PORTD->PCR[4] |= PORT_PCR_MUX(4); // TPM0_CH4 - PWM - Left
	PORTD->PCR[5] |= PORT_PCR_MUX(4); //TPM0_CH5 - encoder
	
	
	// set PHASE pins (GPIO) as output
	PTA->PDDR |= (1uL<<13);
	PTC->PDDR |= (1uL<<9);
	
	
	//////////////////////////////////////// PWM ////////////////////////////////////////
	
	//select clock for TMP0
	
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); // set 'MCGFLLCLK clock or MCGPLLCLK/2'
	
	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set "MCGPLLCLK clock with fixed divide by two"
	
	// set "up-counting"
	TPM0->SC &= ~TPM_SC_CPWMS_MASK; // default set
	
	// divide by 1
	TPM0->SC &= ~TPM_SC_PS_MASK; // the same TPM_SC_PS(0)
	//TPM0->SC |=  TPM_SC_PS(6);
	
	// clear counter
	TPM0->CNT = 0x00; 
	
	// set MOD for PWM period equal 4095 ( 12 bit)
	TPM0->MOD = V_MOD;
	//TPM0->MOD = 60000;
	////////////////////////////////////////////////////
	// Enable general TPM0 interrupt
	//TPM0->SC |= TPM_SC_TOIE_MASK;
	
	
	//SET TPM1 as not so fast timer
	
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	TPM1->SC &= ~TPM_SC_CPWMS_MASK; // default set
	TPM1->SC |= TPM_SC_PS(7); // the same TPM_SC_PS(0)
	TPM1->CNT = 0x00; 
	TPM1->MOD = 1000;
	NVIC_ClearPendingIRQ(TPM1_IRQn);				/* Clear NVIC any pending interrupts on TPM0 */
	NVIC_EnableIRQ(TPM1_IRQn);	
	NVIC_SetPriority(TPM1_IRQn,1);		/* Enable NVIC interrupts source for TPM0 module */
	TPM1->SC |= TPM_SC_CMOD(1);
	TPM1->SC &= ~TPM_SC_TOIE_MASK;	
	
	
	//////////////////////////////////////// ENGINE CHANNELS ////////////////////////////////////////
	
	
	//Right engine
	// set TPM0 channel 2 - "Edge-aligned PWM High-true pulses"
	TPM0->CONTROLS[2].CnSC |= TPM_CnSC_MSB_MASK	
												 |  TPM_CnSC_ELSB_MASK;
													
	// Default value for Right engine
	TPM0->CONTROLS[2].CnV = 0; // STOP
	
	
	//Left engine
	// set TPM0 channel 4 - "Edge-aligned PWM High-true pulses"
	TPM0->CONTROLS[4].CnSC |= TPM_CnSC_MSB_MASK	
												 |  TPM_CnSC_ELSB_MASK;
													
	// Default value for Left engine
	TPM0->CONTROLS[4].CnV = 0; // STOP
	
	
	
	//////////////////////////////////////// ENCODER CHANNELS ////////////////////////////////////////
	
	//Left Engine
	// set TPM0 channel 3 - "Input capture on rising or falling edge"
	TPM0->CONTROLS[3].CnSC |= TPM_CnSC_ELSA_MASK
												 |	TPM_CnSC_ELSB_MASK;
														
	
	//Right Engine
	// set TPM0 channel 5 - "Input capture on rising or falling edge"
	TPM0->CONTROLS[5].CnSC |= TPM_CnSC_ELSA_MASK
											   |	TPM_CnSC_ELSB_MASK;
																												
																												
	//////////////////////////////////////// ENCODER CHANNELS ////////////////////////////////////////
	
	
	NVIC_ClearPendingIRQ(TPM0_IRQn);				/* Clear NVIC any pending interrupts on TPM0 */
	NVIC_EnableIRQ(TPM0_IRQn);							/* Enable NVIC interrupts source for TPM0 module */
	
	NVIC_SetPriority (TPM0_IRQn, PRIORITY_INTERRUPT_DRIVE);	// priority interrupt
	
	// enable counter
	TPM0->SC |= TPM_SC_CMOD(1);
	
}



void setLeftTrackDir(int direction)
{
	
	if(direction == FORWARD)
	{
		PTA->PCOR |= LEFT_PHASE_PIN;
	}
	else if(direction == REVERSE)
	{
		PTA->PSOR |= LEFT_PHASE_PIN;
	}
	
}


void setLeftTrackSpeed(int speed)
{
	
	if( speed >= 0 )
	{
		if( speed < 100 )
		{
			TPM0->CONTROLS[4].CnV = V_MOD * speed/100;
		}
		else if ( speed == 100 )
		{
			TPM0->CONTROLS[4].CnV = V_MOD + 1;
		}
	}
	
}


void setRightTrackDir(int direction)
{
	
	if(direction == FORWARD)
	{
		PTC->PCOR |= RIGHT_PHASE_PIN;
	}
	else if(direction == REVERSE)
	{
		PTC->PSOR |= RIGHT_PHASE_PIN;
	}
	
}


void setRightTrackSpeed(int speed)
{
	
	if( speed >= 0 )
	{
		if( speed < 100 )
		{
			TPM0->CONTROLS[2].CnV = V_MOD * speed/100;
		}
		else if ( speed == 100 )
		{
			TPM0->CONTROLS[2].CnV = V_MOD + 1;
		}
	}
	
}


void setTracksDir(int leftDir, int rightDir)
{
	
	setLeftTrackDir( leftDir );
	setRightTrackDir( rightDir );
	
}


void setTracksSpeed(int leftSpeed, int rightSpeed)
{
	if (leftSpeed>100)leftSpeed=100;
	if (rightSpeed>100)rightSpeed=100;
	setLeftTrackSpeed( leftSpeed );
	setRightTrackSpeed( rightSpeed );
	
}


void driveForward(int speed)
{
	
	if( speed >= 0 && speed <= 100 )
	{
		setTracksDir( FORWARD, FORWARD);
		setTracksSpeed( speed, speed);
	}
	avg_predkosc=speed;
}


void driveReverse(int speed)
{
	
	if( speed >= 0 && speed <= 100 )
	{
	setTracksDir( REVERSE, REVERSE);
	setTracksSpeed( speed, speed);
	}
	avg_predkosc=-speed;
}


void driveStop(void)
{
	
	setTracksSpeed( 0, 0);
	//setTracksDir( FORWARD, FORWARD);
	avg_predkosc=0;
}







void driveAccelerate(int howMuch)
{
	
	// local variables initialization
	uint8_t tmp1 = 0 ;
	uint8_t tmp2 = 0 ;
	uint8_t tmp3 = 0 ;
	
	//Left ENGINE
	tmp1 = ((TPM0->CONTROLS[4].CnV * 100) / V_MOD ) ;
	
	//Right ENGINE
	tmp2 = ((TPM0->CONTROLS[2].CnV * 100 )/ V_MOD) ;
	
	for( tmp3 =0; tmp3 < howMuch ; tmp3++){
		
	      if((tmp1  + howMuch) >= 100){
		
		    TPM0->CONTROLS[4].CnV = V_MOD + 1;
		
	      }
				if((tmp2 + howMuch )>=100 ){
					
					TPM0->CONTROLS[2].CnV = V_MOD + 1;
				}
				if((tmp1 + howMuch) <= 0 ){
				
					TPM0->CONTROLS[4].CnV = 0;
				}
          if((tmp2 + howMuch) <= 0 ){
						
						TPM0->CONTROLS[2].CnV = 0;
					}
					
				if( (tmp1 + howMuch)>0 && (tmp1 + howMuch)<100){
		
	       TPM0->CONTROLS[4].CnV = ((tmp1 + tmp3)*V_MOD/100);
	
         }
				
				 if( (tmp2 + howMuch)>0 && (tmp2 + howMuch)<100){
		
	       TPM0->CONTROLS[2].CnV = ((tmp2 + tmp3)*V_MOD/100);
	
         }
	
				 wait4();
			 }
	
}

int less_or_more=0;
int destination_deg=0;
int now_deg=0;

void turnRightC(int degrees)
{
	ready=0;
		
	left=avg_predkosc+skret_speed;
	right=avg_predkosc-skret_speed;
	prostaJazda = 0;
	setTracksDir((left>0)?FORWARD:REVERSE,(right>0)?FORWARD:REVERSE);
	setTracksSpeed(fabs(left),fabs(right));
	
	//setTracksDir(FORWARD,REVERSE);
	//setTracksSpeed(30,30);
	
	 now_deg= eCompass();
	destination_deg=now_deg-degrees;
	if(destination_deg<0){destination_deg+=360;}
	less_or_more=LESS;
	
	//SET TMP0 ch 0 
	NVIC_ClearPendingIRQ(TPM1_IRQn);	
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHIE_MASK;
	TPM1->CONTROLS[0].CnV = 400;
	
	TPM1->CONTROLS[0].CnSC |= (TPM_CnSC_MSA_MASK
												 |	TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK);
	
	TPM1->CONTROLS[0].CnSC &= ~(TPM_CnSC_ELSA_MASK);
	


}


void turnLeftC(int degrees)
{
	ready=0;
	
	
	left=avg_predkosc-skret_speed;
	right=avg_predkosc+skret_speed;
	prostaJazda = 0;
	setTracksDir((left>0)?FORWARD:REVERSE,(right>0)?FORWARD:REVERSE);
	setTracksSpeed(fabs(left),fabs(right));
	
	//	setTracksDir(REVERSE,FORWARD);
	//	setTracksSpeed(30,30);
	
	 now_deg= eCompass();
	destination_deg=now_deg+degrees;
	if(destination_deg>359){destination_deg-=360;}
	less_or_more=MORE;
	
	//SET TMP0 ch 0
	NVIC_ClearPendingIRQ(TPM1_IRQn);	
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHIE_MASK;
	TPM1->CONTROLS[0].CnV = 400;
	
	TPM1->CONTROLS[0].CnSC |= (TPM_CnSC_MSA_MASK
												 |	TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK);
	
	TPM1->CONTROLS[0].CnSC &= ~(TPM_CnSC_ELSA_MASK);


}


void turnLeftDir(int dir)
{
	ready=0;
	
	
	left=avg_predkosc-skret_speed;
	right=avg_predkosc+skret_speed;
if(poprawka==1){poprawka_skret=1;}else{prostaJazda = 0;}
	setTracksDir((left>0)?FORWARD:REVERSE,(right>0)?FORWARD:REVERSE);
	setTracksSpeed(fabs(left),fabs(right));
	
	//setTracksDir(REVERSE,FORWARD);
	//setTracksSpeed(30,30);
	
	 now_deg= eCompass();
	destination_deg=dir;
	if(destination_deg>359){destination_deg-=360;}
	less_or_more=MORE;
	
	//SET TMP0 ch 0 
	NVIC_ClearPendingIRQ(TPM1_IRQn);	
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHIE_MASK;
	TPM1->CONTROLS[0].CnV = 400;
	
	TPM1->CONTROLS[0].CnSC |= (TPM_CnSC_MSA_MASK
												 |	TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK);
	
	TPM1->CONTROLS[0].CnSC &= ~(TPM_CnSC_ELSA_MASK);
	
	

}

void turnRightDir(int dir)
{
	ready=0;

	left=avg_predkosc+skret_speed;
	right=avg_predkosc-skret_speed;
	
		if(poprawka==1){poprawka_skret=1;}else{prostaJazda = 0;}
	
	setTracksDir((left>0)?FORWARD:REVERSE,(right>0)?FORWARD:REVERSE);
	setTracksSpeed(fabs(left),fabs(right));
	
	
	
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	TPM1->SC &= ~TPM_SC_CPWMS_MASK; // default set
	TPM1->SC |= TPM_SC_PS(7); // the same TPM_SC_PS(0)
	TPM1->CNT = 0x00; 
	TPM1->MOD = 1000;
	NVIC_ClearPendingIRQ(TPM1_IRQn);				/* Clear NVIC any pending interrupts on TPM0 */
	NVIC_EnableIRQ(TPM1_IRQn);	
	NVIC_SetPriority(TPM1_IRQn,1);		/* Enable NVIC interrupts source for TPM0 module */
	TPM1->SC |= TPM_SC_CMOD(1);
	TPM1->SC &= ~TPM_SC_TOIE_MASK;	
		
	//setTracksDir(FORWARD,REVERSE);
	//setTracksSpeed(30,30);
	
	 now_deg= eCompass();
	destination_deg=dir;
	if(destination_deg<0){destination_deg+=360;}
	less_or_more=LESS;

	//SET TMP1 ch 0 
	NVIC_ClearPendingIRQ(TPM1_IRQn);	
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHIE_MASK;
	TPM1->CONTROLS[0].CnV = 400;
	
	TPM1->CONTROLS[0].CnSC |= (TPM_CnSC_MSA_MASK
												 |	TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK);
	
	TPM1->CONTROLS[0].CnSC &= ~(TPM_CnSC_ELSA_MASK);
	
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHIE_MASK;	
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK; 
	TPM1->SC |= TPM_SC_TOF_MASK;
	TPM1->SC |= TPM_SC_TOIE_MASK;

}

void stopTurning(){
	ready=1;
	TPM1->CONTROLS[0].CnSC &= ~TPM_CnSC_CHIE_MASK;	
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK; 
	TPM1->SC |= TPM_SC_TOF_MASK;
	TPM1->SC &= ~TPM_SC_TOIE_MASK;
	//SIM->SCGC6 &= ~SIM_SCGC6_TPM1_MASK;
	
	/*if(poprawka_skret==1){
		poprawka_skret=0;
		//TPM1->CONTROLS[0].CnV = 1000;
	}else{
	TPM1->CONTROLS[0].CnSC &= ~TPM_CnSC_CHIE_MASK;	
	}*/
	setTracksSpeed(fabs(avg_predkosc),fabs(avg_predkosc));
		setTracksDir((avg_predkosc>0)?FORWARD:REVERSE,(avg_predkosc>0)?FORWARD:REVERSE);
	ready=1;
	
}

void TPM1_IRQHandler(void)
{

	
	if((TPM1->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK) == TPM_CnSC_CHF_MASK){
			int temp=eCompass();

		
		
		if(prostaJazda==0 || poprawka_skret==1){
		
	
	
	TPM0->SC |= TPM_SC_TOF_MASK; //kasuje flage przerwania
	
	
	if(less_or_more==LESS){
	if((destination_deg<now_deg && temp>now_deg+20)  || (temp<=destination_deg && (destination_deg<=now_deg ||  (destination_deg>=now_deg && temp>now_deg+20))))stopTurning();
	}else{
	if((destination_deg>now_deg && temp<now_deg-20)  || (temp>=destination_deg && (destination_deg>=now_deg ||  (destination_deg<=now_deg && temp<now_deg-20))))stopTurning();		
	}	
	
		
	
	}else{
		int delta = fabs(eCompass()-kierunek);
		poprawka=1;
		
		if(delta>10 && delta<300){	
				if(temp>kierunek){turnRightDir(kierunek);}else{
					turnLeftDir(kierunek);
				}
			}
		if(delta>300 && delta<350){	
				if(temp>kierunek){turnLeftDir(kierunek);}else{
					turnRightDir(kierunek);
				}	
		}
		
		poprawka=0;
		
	}
	
	
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK; 
	
	}
		
}



void turnLeft(void){
	left=avg_predkosc-skret_speed;
	right=avg_predkosc+skret_speed;
	setTracksDir((left>0)?FORWARD:REVERSE,(right>0)?FORWARD:REVERSE);
	setTracksSpeed(fabs(left),fabs(right));
}

void turnRight(void){
	left=avg_predkosc+skret_speed;
	right=avg_predkosc-skret_speed;
	setTracksDir((left>0)?FORWARD:REVERSE,(right>0)?FORWARD:REVERSE);
	setTracksSpeed(fabs(left),fabs(right));
}
void wyprostuj(void){
	left=avg_predkosc;
	right=avg_predkosc;
	setTracksDir((left>0)?FORWARD:REVERSE,(right>0)?FORWARD:REVERSE);
	setTracksSpeed(fabs(left),fabs(right));
}

void zmienPredkosc(uint8_t speed){
		if( speed >= 0 && speed <= 90 )
	{
		left = left-avg_predkosc+speed;
		right = right-avg_predkosc+speed;
		
		setTracksDir((left>0)?FORWARD:REVERSE,(right>0)?FORWARD:REVERSE);
		setTracksSpeed(fabs(left),fabs(right));
	}
	avg_predkosc=speed;
}

void wait4(void)
{
	
	int i = 0;
	for( i = 0 ; i < 799990 ; i++){}
	
}

uint16_t ifReady(void)
{
	return ready;
}

void notReady(void)
{
	 ready=0;
}


void jedzProsto(void){
	prostaJazda = 1;
	kierunek = eCompass();
	
	NVIC_ClearPendingIRQ(TPM1_IRQn);	
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHIE_MASK;
	TPM1->CONTROLS[0].CnV = 400;
	
	TPM1->CONTROLS[0].CnSC |= (TPM_CnSC_MSA_MASK
												 |	TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK);
	
	TPM1->CONTROLS[0].CnSC &= ~(TPM_CnSC_ELSA_MASK);
}
