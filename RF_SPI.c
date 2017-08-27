#include <p24Fxxxx.h>
#include <stdlib.h>
#include "RF_SPI.h"
#include "RFInit.h"
#define USE_AND_OR /* To enable AND_OR mask setting */
#include <spi.h>

UINT8 check = 0;

void SpiInitDevice()
{
    unsigned int SPIXCON1reg, SPIXCON2reg, SPIxSTATreg;
    SPIXCON1reg = ENABLE_SCK_PIN | ENABLE_SDO_PIN | SPI_MODE8_ON | SPI_SMP_OFF
                  | SPI_CKE_ON | SLAVE_ENABLE_OFF | CLK_POL_ACTIVE_HIGH
                  | MASTER_ENABLE_ON ;
    SPIXCON2reg = FRAME_ENABLE_OFF;
    SPIxSTATreg = SPI_ENABLE;
    OpenSPI1(SPIXCON1reg, SPIXCON2reg, SPIxSTATreg);
}

void RF_SPI_Init(void)
{
    // Initialisation
    TRISBbits.TRISB4 = 0;   // RF_CE
    TRISBbits.TRISB15 = 0;  // RF_CSN
    TRISAbits.TRISA4 = 1;   // RF_IRQ

    TRISBbits.TRISB12 = 0;  // RF_SCK
    TRISBbits.TRISB13 = 0;  // RF_MOSI
    TRISBbits.TRISB14 = 1;  // RF_MISO
    
    // SPI Channel Initialisation
    SpiInitDevice();

    return;
}	


/*****************************************************************************************
Function:       UINT8 SPI_RW_Byte( UINT8 Data )
Parameter: 
                Data     [IN]    Data byte will be write according spi bus
Return: 
                Return byte from spi bus
Description:
                Write a byte to and Read a byte from spi bus
*****************************************************************************************/
UINT8 SPI_RW_Byte( UINT8 Data )
{
    IFS0bits.SPI1IF = 0;
    SPI1BUF = Data;
    while(!IFS0bits.SPI1IF){}
    UINT8 rData = SPI1BUF;

    return rData;
}


/*****************************************************************************************
Function:       UINT8 SPI_Read_Reg( UINT8 reg )
Parameter: 
                reg     [IN]    register address 
Return: 
                The register value 
Description:
                Read from BK2421's register
*****************************************************************************************/
UINT8 SPI_Read_Reg( UINT8 reg )
{
    UINT8 res;

    CLR_CSN();

    SPI_RW_Byte( reg );
    res = SPI_RW_Byte( 0 );

    SET_CSN();
    
    return res;
}


/*****************************************************************************************
Function:       void SPI_Read_Buf(UINT8 reg, UINT8 *pBuffer, UINT8 Length)
Parameter: 
                reg     [IN]    register address
                pBuffer [OUT]   read data buffer
                Length  [IN]    buffer length
Return: 
                None
Description:
                Read multiple bytes from BK2421's register
*****************************************************************************************/
void SPI_Read_Buf(UINT8 reg, UINT8 *pBuffer, UINT8 Length)
{
    CLR_CSN();

    SPI_RW_Byte( reg );

    while( Length -- )
    {
        *pBuffer++ = SPI_RW_Byte( 0 );
    }

    SET_CSN();
}


/*****************************************************************************************
Function:       void SPI_Write_Reg(UINT8 reg, UINT8 value)
Parameter: 
                reg     [IN]    register address
                value   [IN]    value's for register
Return: 
                None 
Description:
                Write to BK2421's register
*****************************************************************************************/
void SPI_Write_Reg(UINT8 reg, UINT8 value)
{
    CLR_CSN();

    SPI_RW_Byte( reg );
    check = SPI_RW_Byte( value );

    SET_CSN();
}


/*****************************************************************************************
Function:       void SPI_Write_Buf(UINT8 reg, UINT8 *pBuffer, UINT8 Length)
Parameter: 
                reg     [IN]    register address
                pBuffer [IN]   write data buffer
                Length  [IN]    buffer length
Return: 
                None
Description:
                Write multiple bytes to BK2421's register
*****************************************************************************************/
void SPI_Write_Buf(UINT8 reg, UINT8 *pBuffer, UINT8 Length)
{
    CLR_CSN();

    SPI_RW_Byte( reg );

    while( Length-- )
    {
        SPI_RW_Byte( *pBuffer++ );
    }

    SET_CSN();
}



