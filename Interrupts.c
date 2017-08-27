#include <p24Fxxxx.h>
#include <i2c.h>
#include <spi.h>


//******************** Interrupt sevice rotuine for I2C1 Master ***************************
void __attribute__((interrupt,no_auto_psv)) _MI2C1Interrupt(void)
{
  MI2C1_Clear_Intr_Status_Bit;  //Clear Interrupt status of I2C1
}

//******************** Interrupt sevice rotuine for SPI1 Master ***************************
void __attribute__((interrupt,no_auto_psv)) _SPI1Interrupt(void)
{
  SPI1_Clear_Intr_Status_Bit; //Clear Interrupt status of SPI1
}


