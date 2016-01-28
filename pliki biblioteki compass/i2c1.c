/** \file i2c.h
	\brief Obsluga komunikacji I2C1
*/
#include "i2c.h"

/** \fn void i2c1_set_tx_mode(void)
	\brief Funkcja ustawiajaca tryb wysylania
*/
void i2c1_set_tx_mode(void)
{
    I2C1->C1 |= I2C_C1_TX_MASK;

}

/** \fn void i2c1_set_rx_mode(void)
	\brief Funkcja ustawiajaca tryb odbierania
*/
void i2c1_set_rx_mode(void)
{
    I2C1->C1 &= ~I2C_C1_TX_MASK;
}

/** \fn void i2c1_set_slave_mode(void)
	\brief Funkcja ustawiajaca tryb slave
*/
void i2c1_set_slave_mode(void)
{
    I2C1->C1  &= ~I2C_C1_MST_MASK;
}

/** \fn void i2c1_set_master_mode(void)
	\brief Funkcja ustawiajaca tryb master
*/
void i2c1_set_master_mode(void)
{
    I2C1->C1  |=  I2C_C1_MST_MASK;
}

/** \fn void i2c1_give_nack(void)
	\brief Funkcja wysylajaca negarywne potwierdzenie
*/
void i2c1_give_nack(void)
{
    I2C1->C1 |= I2C_C1_TXAK_MASK;
}

/** \fn void i2c1_give_ack(void)
	\brief Funkcja wysylajaca pozytywne potwierdzenie
*/
void i2c1_give_ack(void)
{
    I2C1->C1 &= ~I2C_C1_TXAK_MASK;
}

/** \fn void i2c1_repeated_start(void)
	\brief Funkcja wywolujaca ponowny start transmisji
*/
void i2c1_repeated_start(void)
{
    I2C1->C1     |= 0x04;
}

/** \fn void i2c1_write_byte(uint8_t data)
	\brief Funkcja wpisujaca wyslane dane
	\param data Dane do wpisana
*/
void i2c1_write_byte(uint8_t data)
{
    I2C1->D = data;
}

/** \fn uint8_t i2c1_read_byte(void)
	\brief Funkcja zwracajaca odczytane dane
	\return Zwraca przeczytany bajt
*/
uint8_t i2c1_read_byte(void)
{
    return I2C1->D;
}

/** \fn void i2c1_start(void)
	\brief Funkcja rozpoczynajaca wysylanie
*/
void i2c1_start(void)
{
    i2c1_set_master_mode();
    i2c1_set_tx_mode();
}

/** \fn void i2c1_stop(void)
	\brief Funkcja konczaca wysylanie, rozpoczynajaca odbieranie
*/
void i2c1_stop(void)
{
    i2c1_set_slave_mode();
    i2c1_set_rx_mode();
}

/** \fn void i2c1_wait()
	\brief Funkcja czekajaca 
*/
void i2c1_wait()
{
    // wait flag
    while((I2C1->S & I2C_S_IICIF_MASK)==0);
    // clear flag
    I2C1->S |= I2C_S_IICIF_MASK;
}

/** \fn uint16_t i2c1_get_ack()
	\brief Funkcja zwracajaca 1 po otrzymaniu potwierdzenia
*/
uint16_t i2c1_get_ack()
{
    if((I2C1->S & I2C_S_RXAK_MASK) == 0)
        return 1;
    else
        return 0;
}


/**	\brief Funkcja wylaczajaca modul I2C1 */
void hal_i2c1_deinit(void)
{
    I2C1->C1 = 0x00;
    
    SIM->SCGC4 &= ~SIM_SCGC4_I2C1_MASK;
}

/** \brief Funkcja zalaczajaca modul I2C1 */
void hal_i2c1_init(void)  
{
	SIM->SCGC4 |= SIM_SCGC4_I2C1_MASK;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

  PORTE->PCR[0] |= PORT_PCR_MUX(6);
  PORTE->PCR[1] |= PORT_PCR_MUX(6);
	
	 I2C1->F  = 0x14; // baudrate
   I2C1->C1 = 0x80; // enable IIC
}

