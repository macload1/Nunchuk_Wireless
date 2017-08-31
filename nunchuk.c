#include <p24Fxxxx.h>
#include <stdlib.h>

#define USE_AND_OR /* To enable AND_OR mask setting */
#include <i2c.h>

#include "nunchuk.h"
#include "TimeDelay.h" 

extern volatile unsigned char I2CData[];

/*
Function: i2c_init
Return:
Arguments:
Description: Initialize I2C in master mode, Sets the required baudrate
*/
void i2c_init(void)
{
    UINT16 config1 = 0;
    UINT16 config2 = 0;
    
    TRISBbits.TRISB8 = 1;       // Configure SCL1 to input
    TRISBbits.TRISB9 = 1;       // Configure SDA1 to input
    /* Turn off I2C modules */
    CloseI2C1();                //Disbale I2C1 mdolue if enabled previously
    /* Turn off interrupts */
    ConfigIntI2C1(MI2C_INT_OFF);    //Disable I2C interrupt

    //***************** I2C1 configuration *************************************
    /***************************************************************************
    *
    *        I2C1 enabled
    *        continue I2C module in Idle mode
    *        IPMI mode not enabled
    *        I2CADD is 7-bit address
    *        Disable Slew Rate Control for 100KHz
    *        Enable SM bus specification
    *        Disable General call address
    *        Baud @ 8MHz = 39 into I2CxBRG
    ***************************************************************************/
    config1 = (I2C_ON | I2C_7BIT_ADD );
    config2 = 39;
    OpenI2C1(config1,config2);      //configure I2C1
    
    return;
}

/*
Function: i2c_wait
Return:
Arguments:
Description: wait for transfer to finish
*/
void i2c_wait(void)
{
    IdleI2C1();
    /* wait for any pending transfer */
}

/*
Function: i2c_restart
Return:
Arguments:
Description: Sends a repeated start condition on I2C Bus
*/
void i2c_restart(void)
{
    RestartI2C1();
    IdleI2C1();
}

/*
Function: i2c_ack
Return:
Arguments:
Description: Generates acknowledge for a transfer
*/
void i2c_ack(void)
{
    AckI2C1();
    IdleI2C1();
}

/*
Function: i2c_nak
Return:
Arguments:
Description: Generates Not-acknowledge for a transfer
*/
void i2c_nak(void)
{
    NotAckI2C1();
    IdleI2C1();
}

/*
Function: i2c_start
Return:
Arguments:
Description: Send a start condition on I2C Bus
*/
void i2c_start(void)
{
    //************** Start condition *******************************************
    IdleI2C1();
    StartI2C1();
    while(I2C1CONbits.SEN );     // Wait till Start sequence is completed
    MI2C1_Clear_Intr_Status_Bit; // Clear interrupt flag
}

/*
Function: i2c_send
Return:
Arguments: dat - 8-bit data to be sent on bus
        data can be either address/data byte
Description: Send 8-bit data on I2C bus
*/
void i2c_send(UINT8 data)
{
    MasterWriteI2C1(data);       // Write data for transmission
    while(I2C1STATbits.TBF);     // Wait till address is transmitted
    while(!IFS1bits.MI2C1IF);    // Wait for ninth clock cycle
    MI2C1_Clear_Intr_Status_Bit; // Clear interrupt flag
    while(I2C1STATbits.ACKSTAT);
}

/*
Function: i2c_read
Return:    8-bit data read from I2C bus
Arguments:
Description: read 8-bit data from I2C bus
*/
unsigned char i2c_read(void)
{
    UINT8 temp;
    /* Reception works if transfer is initiated in read mode */
    temp = MasterReadI2C1();    // Read data from I2C slave
    i2c_wait();                 // Wait to check any pending transfer
    return temp;                // Return the read data from bus
}

/*
Function: i2c_stop
Return:
Arguments:
Description: Send a stop condition on I2C Bus
*/
void i2c_stop(void)
{
    //************** Stop condition ********************************************
    IdleI2C1();              // wait for the I2C to be Idle
    StopI2C1();              // Terminate communication protocol with stop signal
    while(I2C1CONbits.PEN);  // Wait till stop sequence is completed
}

// Nunchuk Init: Initialize the nunchuk by sending I2C commands to make the
// nunchuk "active". This particlular routine is different than others, but
// this one works with generic/knockoffs such as the one I'm using.
void nunchuk_init(void)
{
    // I2C Init
    i2c_init();

    /* Send Start condition */
    i2c_start();
    /* Transmit to device 0x52 */
    i2c_send(0xA4);
    i2c_send(0x00);
    /* Send a stop condition - as transfer finishes */
    i2c_stop();

    DelayMs(30);

    /* Send Start condition */
    i2c_start();
    /* Transmit to device 0x52 */
    i2c_send(0xA4);
    i2c_send(0x00);
    /* Send a stop condition - as transfer finishes */
    i2c_stop();

    DelayMs(30);

    /* Send Start condition */
    i2c_start();
    /* Transmit to device 0x52 */
    i2c_send(0xA4);
    i2c_send(0x00);
    /* Send a stop condition - as transfer finishes */
    i2c_stop();

    DelayMs(30);

    /* Send Start condition */
    i2c_start();
    /* Transmit to device 0x52 */
    i2c_send(0xA4);
    i2c_send(0xF0);
    i2c_send(0x55);
    /* Send a stop condition - as transfer finishes */
    i2c_stop();

    DelayMs(30);

    /* Send Start condition */
    i2c_start();
    /* Transmit to device 0x52 */
    i2c_send(0xA4);
    i2c_send(0xFB);
    i2c_send(0x00);
    /* Send a stop condition - as transfer finishes */
    i2c_stop();

    DelayMs(30);

    /* Send Start condition */
    i2c_start();
    /* Transmit to device 0x52 */
    i2c_send(0xA4);
    i2c_send(0x40);
    i2c_send(0x00);
    /* Send a stop condition - as transfer finishes */
    i2c_stop();

    DelayMs(30);

    /* Send Start condition */
    i2c_start();
    /* Transmit to device 0x52 */
    i2c_send(0xA4);
    i2c_send(0x00);
    /* Send a stop condition - as transfer finishes */
    i2c_stop();

    DelayMs(30);
}


void send_request(void)
{
    /* Send Start condition */
    i2c_start();
    /* Transmit to device 0x52 */
    i2c_send(0xA4);
    /* Sends a zero */
    i2c_send(0x00);
    /* Send a stop condition - as transfer finishes */
    i2c_stop();
}


// Nunchuk Read: Read the six bytes of data from the nunchuk. The read must be
// preceded by sending a zero to the nunchuk.
void nunchuk_read(void)
{
    /* Send data request */
    send_request();

    Delay10us(20);

    /* Send START condition */
    i2c_start();
    i2c_send(0xA5);
    I2CData[0] = i2c_read();
    /* ACK if it's not the last byte to read */
    i2c_ack();
    I2CData[1] = i2c_read();
    /* ACK if it's not the last byte to read */
    i2c_ack();
    I2CData[2] = i2c_read();
    /* ACK if it's not the last byte to read */
    i2c_ack();
    I2CData[3] = i2c_read();
    /* ACK if it's not the last byte to read */
    i2c_ack();
    I2CData[4] = i2c_read();
    /* ACK if it's not the last byte to read */
    i2c_ack();
    I2CData[5] = i2c_read();
    /* NAK if it's the last byte to read */
    i2c_nak();
    /* Send STOP condition */
    i2c_stop();
    
    
/*
    I2CData[0] = (I2CData[0] ^ 0x17) + 0x17;
    I2CData[1] = (I2CData[1] ^ 0x17) + 0x17;
    I2CData[2] = (I2CData[2] ^ 0x17) + 0x17;
    I2CData[3] = (I2CData[3] ^ 0x17) + 0x17;
    I2CData[4] = (I2CData[4] ^ 0x17) + 0x17;
    I2CData[5] = (I2CData[5] ^ 0x17) + 0x17;
 */
}