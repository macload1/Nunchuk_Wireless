#include "GenericTypeDefs.h"

#include "Bk_rxtx.h"
#include "RFInit.h"

#include "TimeDelay.h"
#include "RF_SPI.h"

#include <string.h> /* memset */

volatile RFPKT RF;
volatile RFDATAPKT g_RFRecData;         //RF receive data package;
volatile RFDATAPKT g_RFSendData;		//RF send data package;

extern volatile UINT32 Bk2423_Bank1_Reg0_13[];

extern volatile unsigned char I2CData[];

/****************************************************************************
Function:             	void RF_SendData( UINT8 *pSnd, UINT8 Len )
Parameter:
                        pData	[IN]	RF send buffer
                        Len		[IN]	Send valid length
Return:
                        None
Description:
                        RF send data and wait with send complete
****************************************************************************/
void RF_SendData( UINT8 *pSnd, UINT8 Len )
{
    UINT8 Rt =   0x00;

    SwitchtoTXMode();
    //Writes Data to TX FIFO;
    CLR_CE();
    SPI_Write_Buf( W_TX_PAYLOAD , pSnd , Len );
    	                    
    //Wait Send;Require Maintain Time > 20us;
    SET_CE();
    Delay10us(6);   //_delay_us( 60 );						                   
    CLR_CE();
    
    RF.VlidIRQ          =   0x00;
    RF.SnTimeOutCnt     =   0x00;

    //Wait Send complete
    while( 1 )
    {
        if( RF.VlidIRQ )
        {
            RF.VlidIRQ  =   0x00;                                       //clear interrupt flag;
            Rt = SPI_Read_Reg( R_REGISTER | STATUS );                   //Read Status Register
                                         
            if( (Rt & STATUS_TX_DS) || (Rt & STATUS_MAX_RT) )           //TX IRQ?
            {
            if( Rt & STATUS_MAX_RT )                                    //if send fail
            {
            SPI_Write_Reg( FLUSH_TX, 0X00 ); 				            //Clear TX FIFO;
            }
            
            SPI_Write_Reg( FLUSH_RX, 0X00 ); 				            //Clear TX FIFO;
            SPI_Write_Reg(W_REGISTER | STATUS,Rt);                      //Clear interrupt flag;
            break;
            }
        }
        else if( RF.SnTimeOutCnt    >=  10 )                            //Send Timeout >= 10ms;
        {
            SPI_Write_Reg( FLUSH_TX, 0X00);                             //Clear TX FIFO;
            break;
        }
    }
    SwitchtoRXMode( );
}

/****************************************************************************
Function:             	UINT8 RF_RecvData( void )
Parameter:
                        None
Return:
                        None
Description:
                        
****************************************************************************/
UINT8 RFRecvPacket( void )
{
    UINT8  Rt=0,Len=0, rcok= 0, temp= 0;
    
    if( RF.VlidIRQ )
    {
        Rt     = SPI_Read_Reg( R_REGISTER | STATUS );    //read interrupt flag;
        if( Rt & STATUS_RX_DR )                        //Happen RX receive interrupt?
        {
            Len   =  Read_RXPayload(( UINT8 *)&g_RFRecData ,sizeof( RFDATAPKT ));
            temp  =  Check_Sum( (UINT8 *)&g_RFRecData , Len );
            
            if( !temp )  
            {
            rcok  =   TRUE;
            }
        }

        if( Rt & STATUS_MAX_RT ) 					   //Send Fail?
        {
            SPI_Write_Reg( FLUSH_TX, 0X00 ); 			   //Clear TX FIFO;
        }
        
        RF.VlidIRQ  =   0x00;
        SPI_Write_Reg( FLUSH_RX, 0X00 ); 			   //Clear RX FIFO;
        SPI_Write_Reg( W_REGISTER | STATUS, Rt ); 	    //Clear interrupt flag;
    }

    return rcok;
}
/***********************************************************************************
Function:           void Fill_DataPkt( UINT8 cmd )
Parameter:
                    None
Return:
                    None
Description:
                    Fill data to RF Send packet
*************************************************************************************/
void FillDataPacket( UINT8 Cmd , UINT8 Type)
{
    switch( Type )  
    {
    case 0x01:          //Send Packet Length = 4+0
        g_RFSendData.Length  =   4;
        break;
    case 0x02:           //Send Packet Length = 4+6  
        g_RFSendData.Length   =   10;       
        //memset( (UINT8*)&g_RFSendData.Param[0] , 0x58, 6 );
        g_RFSendData.Param[0] = I2CData[0];
        g_RFSendData.Param[1] = I2CData[1];
        g_RFSendData.Param[2] = I2CData[2];
        g_RFSendData.Param[3] = I2CData[3];
        g_RFSendData.Param[4] = I2CData[4];
        g_RFSendData.Param[5] = I2CData[5];
        break;
    case 0x03:           //Send Packet Length = 4+16
        g_RFSendData.Length  =   20;      
        memset( (UINT8*)&g_RFSendData.Param[0] , 0xa2, 16 );          
        break;
    }

    g_RFSendData.Cmd      =  Cmd;
    g_RFSendData.Sn      +=  0x01; 

    g_RFSendData.ChckSum  =  0x00;
    g_RFSendData.ChckSum  =  Check_Sum((UINT8 *)&g_RFSendData ,g_RFSendData.Length);
}
 

/**************************************************************************
Function:           void SPI_Bank1_Write_Reg(UINT8 reg, UINT8 *pBuf)
Parameter: 
                    Reg     [IN]
                    pBuff   [IN]
Return:
                   
Description:
                   write a Bank1 register
***************************************************************************/        
void SPI_Bank1_Write_Reg(UINT8 reg, UINT8 *pBuf)    
{
	SwitchBANK( 0x01 );      //Switch to BANK1

	SPI_Write_Buf( reg , pBuf , 0x04 );

	SwitchBANK( 0x00 );      //Switch to BANK0
}
/**************************************************************************
Function:           void SPI_Bank1_Read_Reg(UINT8 reg, UINT8 *pBuf)
Parameter: 
                    Reg     [IN]
                    pBuff   [OUT]
Return:
                   
Description:
                   Read a Bank1 register
***************************************************************************/        
void SPI_Bank1_Read_Reg(UINT8 reg, UINT8 *pBuf)
{
	SwitchBANK( 0x01 );      //Switch to BANK1

	SPI_Read_Buf( reg , pBuf , 4 );

	SwitchBANK( 0x00 );      //Switch to BANK0
}
/**************************************************************************
Function:           UINT8 Get_ChipID( void )
Parameter: 
                    None 
Return:
                    ChipId
Description:
                    Read Chip ID 
***************************************************************************/             
UINT8 Get_ChipID( void )
{
	UINT8 ReadArr[4] = { 0x00 };

	SPI_Bank1_Read_Reg( 0x08 , ReadArr);

	return ReadArr[0];
}
/**************************************************************************
Function:           void Set_LowPowerMode( void )
Parameter: 
                    None
Return:
                    None             
Description:
                    set low power mode
***************************************************************************/             
void Set_LowPowerMode( void )
{
	UINT8 j , WriteArr[4];

	for( j = 0x00; j < 0x04; j++ )
    {
    WriteArr[j]=(Bk2423_Bank1_Reg0_13[4]>>(8*(j) ) )&0xff;
    }
    //REG4<20>
	WriteArr[1] =   WriteArr[1] &   0xef; 

    //write REG4
	SPI_Bank1_Write_Reg(W_REGISTER|0x04,WriteArr);
}

/**************************************************************************
Function:              	void Write_TXPayload( UINT8 * pBuff, UINT8 Len ) 
Parameter: 
                        pBuff	[IN]	Write data of pBuff  to RX FIFO
						Len		[IN]	Write data length	
Return: 
                        None
Description: 
                        Write multiple byte to TX FIFO 
**************************************************************************/
void Write_TXPayload( UINT8 * pBuff, UINT8 Len ) 
{
    SPI_Write_Buf( W_TX_PAYLOAD , pBuff , Len );
}

/************************************************************************** 
Function:              	UINT8 Read_RXPayload( UINT8 * pBuff, UINT8 MaxLen ) 
Parameter: 
                        pBuff	[OUT]	Read multiple byte to pBuff
						MaxLen	[IN	]	Data of  maximum length
Return: 
                        None
Description: 
                        Read multiple byte from RX FIFO
**************************************************************************/
UINT8 Read_RXPayload( UINT8* pBuff, UINT8 MaxLen )
{
	UINT8 Len = 0;

	Len = SPI_Read_Reg( R_RX_PLY_WID );
	if( Len > MaxLen )
	{
		Len = MaxLen;
	}
	SPI_Read_Buf( R_RX_PAYLOAD, pBuff, Len );

	return ( Len );
}


/****************************************************************************
Func:              		UINT8 Check_Sum( UINT8 *pBuff , UINT8 Len )
Parameter:
                        None
Return:
                        None
Description:
                       
*****************************************************************************/
UINT8 Check_Sum( UINT8 *pBuff , UINT8 Len )
{
    UINT8 sum = 0x00;

    while( Len-- )
    {
        sum +=( *pBuff++ );
    }

    return ( ~sum );
}