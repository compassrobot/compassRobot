#ifndef __MAG2_H__
#define __MAG2_H__

#include "MKL46Z4.h"
#include "i2c1.h"

//Rejestry magnetometru //
#define MAG2_DR_STATUS 0x00
#define MAG2_OUT_X_MSB 0x01
#define MAG2_OUT_X_LSB 0x02
#define MAG2_OUT_Y_MSB 0x03
#define MAG2_OUT_Y_LSB 0x04
#define MAG2_OUT_Z_MSB 0x05
#define MAG2_OUT_Z_LSB 0x06
#define MAG2_WHO_AM_I  0x07
#define MAG2_SYSMOD    0x08
#define MAG2_OFF_X_MSB 0x09
#define MAG2_OFF_X_LSB 0x0A
#define MAG2_OFF_Y_MSB 0x0B
#define MAG2_OFF_Y_LSB 0x0C
#define MAG2_OFF_Z_MSB 0x0D
#define MAG2_OFF_Z_LSB 0x0E
#define MAG2_DIE_TEMP  0x0F
#define MAG2_CTRL_REG1 0x10
#define MAG2_CTRL_REG2 0x11


void mag2_init(void);
static void pause(void);

uint8_t   hal_dev_mag2_read_reg(uint8_t addr);
void hal_dev_mag2_write_reg(uint8_t addr, uint8_t data);

short iMag2ReadX(void);
short iMag2ReadY(void);
short iMag2ReadZ(void);



#endif //__I2C_H__