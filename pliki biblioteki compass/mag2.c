/** \file "mag2.h"
\brief Niskopoziomowa obsluga kompasu. */


#include "mag2.h"


signed short Mresult2x, Mresult2y, Mresult2z;

#define MAG22_I2C2_ADDRESS (0x1D<<1)



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
uint8_t hal_dev_mag2_read_reg(uint8_t addr)
{
    uint8_t result2;
	
	__disable_irq();
	
    i2c1_start();
    i2c1_write_byte(MAG22_I2C2_ADDRESS | I2C1_WRITE);
    
    i2c1_wait();
		i2c1_get_ack();

    i2c1_write_byte(addr);
    i2c1_wait();
		i2c1_get_ack();
	
    i2c1_repeated_start();
    i2c1_write_byte(MAG22_I2C2_ADDRESS | I2C1_READ);
    i2c1_wait();
		i2c1_get_ack();
	
    i2c1_set_rx_mode();

    i2c1_give_nack();
    result2 = i2c1_read_byte();
    i2c1_wait();
		i2c1_get_ack();

    i2c1_stop();
    result2 = i2c1_read_byte();
    pause();
		__enable_irq();
    return result2;
}

/**
	\brief Funkcja wpisujaca wartosc do rejestru magnetometru
	\param addr Adres rejestru do ktorego zostanie wpisana dana 
	\param data Dane wpisywane do rejestru
*/
void hal_dev_mag2_write_reg(uint8_t addr, uint8_t data)
{
	__disable_irq();
	
    i2c1_start();

    i2c1_write_byte( MAG22_I2C2_ADDRESS|I2C1_WRITE);
    i2c1_wait();
		i2c1_get_ack();

    i2c1_write_byte(addr);
    i2c1_wait();
		i2c1_get_ack();
	
		i2c1_write_byte(data);
    i2c1_wait();
		i2c1_get_ack();
	
    i2c1_stop();
	
	__enable_irq();
    pause();
}

/** \brief Inicjalizacja Mag2netometru */	
void mag2_init(void)
{
	
		hal_i2c1_init();     
     
		//init
	 // 0x0C = 0b00001100
    // DO = 011 (7.5 Hz ODR)
   // hal_dev_mag3110_write_reg(0x00, 0x0C);

    // 0x20 = 0b00100000
    // GN = 001 (+/- 1.3 gauss full scale)
    hal_dev_mag2_write_reg(0x25, 0x60);//CTRL6
    // 0x00 = 0b00000000
    // MD = 00 (continuous-conversion mode)
		hal_dev_mag2_write_reg(0x26, 0x00); //CTRL7
		hal_dev_mag2_write_reg(0x24, 0x70); //CTRL5
}


/** \brief Funkcja zwracajaca wartosc osi X */	
signed short iMag2ReadX(void) {

      Mresult2x   = hal_dev_mag2_read_reg(0x09)<<8;
      Mresult2x  |= hal_dev_mag2_read_reg(0x08);

    return Mresult2x;
}


/** \brief Funkcja zwracajaca wartosc osi Y */	
signed short iMag2ReadY(void) {

      Mresult2y   = hal_dev_mag2_read_reg(0x0B)<<8;
      Mresult2y  |= hal_dev_mag2_read_reg(0x0A);      

    return Mresult2y;
}


/** \brief Funkcja zwracajaca wartosc osi Z */	
signed short iMag2ReadZ(void) {

      Mresult2z   = hal_dev_mag2_read_reg(0x0D)<<8;
      Mresult2z  |= hal_dev_mag2_read_reg(0x0C);
	
    return Mresult2z;
}

