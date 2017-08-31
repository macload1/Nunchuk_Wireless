#include <p24Fxxxx.h>
#include <i2c.h>
#include <spi.h>
#include <timer.h>

#include "Bk_rxtx.h"


extern volatile RFPKT RF;

//*************** Interrupt sevice rotuine for I2C1 Master *********************
void __attribute__((interrupt,no_auto_psv)) _MI2C1Interrupt(void)
{
    MI2C1_Clear_Intr_Status_Bit;  //Clear Interrupt status of I2C1
}

//*************** Interrupt sevice rotuine for SPI1 Master *********************
void __attribute__((interrupt,no_auto_psv)) _SPI1Interrupt(void)
{
    SPI1_Clear_Intr_Status_Bit; //Clear Interrupt status of SPI1
}

//***************** Interrupt sevice rotuine for Timer1 ************************
void __attribute__((interrupt,no_auto_psv)) _T1Interrupt(void)
{
    T1_Clear_Intr_Status_Bit; //Clear Interrupt status of Timer1
    RF.RcTimeOutCnt++;        //RF Receive Overtime Count
    RF.SnTimeOutCnt++;        //RF Send Overtime Count
}

//********** Interrupt sevice rotuine for Change Notification 0 ****************
void __attribute__((interrupt,no_auto_psv)) _CNInterrupt(void)
{
    IFS1bits.CNIF = 0;      // Clear Change Notification Interrupt Flag
    RF.VlidIRQ = TRUE;      // RF IRQ
}

