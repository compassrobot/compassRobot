#ifndef buttons_h
#define buttons_h

#include "MKL46Z4.h"                    // Device header





void buttonsInitialize(void);
void buttonsService(void);							//buttons' interrupt service procedure
int32_t buttonRead(void);
int getMode(void);

#endif
