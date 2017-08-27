#ifndef _RF_SPI_H
#define _RF_SPI_H

#include "GenericTypedefs.h"
#include <p24Fxxxx.h>

#define	RF_CE		LATBbits.LATB4
#define	RF_CSN		LATBbits.LATB15
#define	RF_IRQ		LATAbits.LATA4

#define SET_CE()        {RF_CE = 1;}
#define CLR_CE()        {RF_CE = 0;}

#define SET_CSN()       {RF_CSN = 1;}
#define CLR_CSN()       {RF_CSN = 0;}

//EXTERN FUNCTIONS
void RF_SPI_Init(void);
UINT8 SPI_Read_Reg( UINT8 reg );
void SPI_Read_Buf(UINT8 reg, UINT8 *pBuffer, UINT8 Length);
void SPI_Write_Reg(UINT8 reg, UINT8 value);
void SPI_Write_Buf(UINT8 reg, UINT8 *pBuffer, UINT8 Length);

#endif  // _RF_SPI_H
