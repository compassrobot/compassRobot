#ifndef __I2C1_H__
#define __I2C1_H__

#include "MKL46Z4.h"

/* ----------------------------------------------------------------------------
   -- I2C
   ---------------------------------------------------------------------------- */


void i2c1_set_tx_mode(void);
void i2c1_set_rx_mode(void);
void i2c1_set_slave_mode(void);
void i2c1_set_master_mode(void);
void i2c1_give_nack(void);
void i2c1_give_ack(void);
void i2c1_repeated_start(void);
void i2c1_write_byte(uint8_t data);
uint8_t i2c1_read_byte(void);
void i2c1_start(void);
void i2c1_stop(void);
void i2c1_wait(void);
uint16_t i2c1_get_ack(void);
void hal_i2c1_deinit(void);
void hal_i2c1_init(void); 

#define I2C1_READ  1
#define I2C1_WRITE 0

#endif //__I2C_H__
