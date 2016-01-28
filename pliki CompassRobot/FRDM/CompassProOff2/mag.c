/** \file "mag.h"
\brief Niskopoziomowa obsluga kompasu. */




#include "mag.h"


signed short Mresultx, Mresulty, Mresultz;

#define MAG3110_I2C_ADDRESS (0x0E<<1)


/** 	\brief Inicjalizacja magnetometru */
void hal_dev_mag3110_init(void)
{
  hal_i2c0_init();     
}

/** \brief Funkcja opozniajaca zapis i odczyt */
static void pause(void)
{    int n;
    for(n=0; n<40; n++){}
}

/**
	\brief Funkcja zwracajaca wartosc rejestru magnetometru
	\param addr Adres odczytywanego rejestru
	\return Odczytana wartosc rejestru magnetometru
*/
uint8_t hal_dev_mag3110_read_reg(uint8_t addr)
{
    uint8_t result;
	
	__disable_irq();
	
    i2c0_start();
    i2c0_write_byte(MAG3110_I2C_ADDRESS | I2C_WRITE);
    
    i2c0_wait();
		i2c0_get_ack();

    i2c0_write_byte(addr);
    i2c0_wait();
		i2c0_get_ack();
	
    i2c0_repeated_start();
    i2c0_write_byte(MAG3110_I2C_ADDRESS | I2C_READ);
    i2c0_wait();
		i2c0_get_ack();
	
    i2c0_set_rx_mode();

    i2c0_give_nack();
    result = i2c0_read_byte();
    i2c0_wait();
		i2c0_get_ack();

    i2c0_stop();
    result = i2c0_read_byte();
    pause();
		__enable_irq();
    return result;
}

/**
	\brief Funkcja wpisujaca wartosc do rejestru magnetometru
	\param addr Adres rejestru do ktorego zostanie wpisana dana 
	\param data Dane wpisywane do rejestru
*/
void hal_dev_mag3110_write_reg(uint8_t addr, uint8_t data)
{
	__disable_irq();
	
    i2c0_start();

    i2c0_write_byte( MAG3110_I2C_ADDRESS|I2C_WRITE);
    i2c0_wait();
		i2c0_get_ack();

    i2c0_write_byte(addr);
    i2c0_wait();
		i2c0_get_ack();
	
		i2c0_write_byte(data);
    i2c0_wait();
		i2c0_get_ack();
	
    i2c0_stop();
	
	__enable_irq();
    pause();
}

/** \brief Inicjalizacja Magnetometru */	
void mag_init(void)
{
    hal_dev_mag3110_init();     
		hal_dev_mag3110_write_reg(MAG_CTRL_REG1,0x00); 
    hal_dev_mag3110_write_reg(MAG_CTRL_REG2,0x80); 
    hal_dev_mag3110_write_reg(MAG_CTRL_REG1,0x01); 
	
}


/** \brief Funkcja zwracajaca wartosc osi X */	
signed short iMagReadX(void) {
   if((hal_dev_mag3110_read_reg(0x00)&0xf) != 0)
    {
      Mresultx   = hal_dev_mag3110_read_reg(0x01)<<8;
      Mresultx  |= hal_dev_mag3110_read_reg(0x02);
    }
    return Mresultx;
}


/** \brief Funkcja zwracajaca wartosc osi Y */	
signed short iMagReadY(void) {
     if((hal_dev_mag3110_read_reg(0x00)&0xf) != 0)
    {
      Mresulty   = hal_dev_mag3110_read_reg(0x03)<<8;
      Mresulty  |= hal_dev_mag3110_read_reg(0x04);      
    }
    return Mresulty;
}


/** \brief Funkcja zwracajaca wartosc osi Z */	
signed short iMagReadZ(void) {
       if((hal_dev_mag3110_read_reg(0x00)&0xf) != 0)
    {
      Mresultz   = hal_dev_mag3110_read_reg(0x05)<<8;
      Mresultz  |= hal_dev_mag3110_read_reg(0x06);
    }
    return Mresultz;
}
