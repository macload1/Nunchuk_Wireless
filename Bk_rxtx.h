#ifndef _BK_RXTX_H
#define _BK_RXTX_H

/*Parameter Length definition*/
#define PARAM_LEN		16			         // Valid Parameter Max for 26
#define PKT_LENGTH      PARAM_LEN+4          //RF最大包长       

/*Interrupt Status Seting Command*/
#define  STATUS_MAX_RT		    0x10   	//Arrived max Retransmits Number;
#define  STATUS_TX_DS 		    0x20    //Send succeed flag;
#define  STATUS_RX_DR		    0x40    //Receive succeed flag;

/*Communication channel choice*/
#define PW_UP                   0x01
#define PW_DN                   0x00
#define BANK_0                  0x00
#define BANK_1                  0x01

//Output Power Command Definition
#define BK2423_P3               0x06
#define BK3243_N2               0x04
#define BK2423_N7               0x02
#define BK2423_N15              0x00

//Data Rate Command Definition
#define BK2423_RATE_250K        0x20
#define BK2423_RATE_1M          0x00
#define BK2423_RATE_2M          0x28

/*External Variable States*/
typedef struct
{
    UINT8   Mode;
    UINT8   VlidIRQ;

    UINT8   SnTimeOutCnt;
    UINT8   RcTimeOutCnt;	
}RFPKT;



typedef struct
{
	UINT8   ChckSum;
	UINT8   Cmd;
	UINT8	Sn;
    UINT8   Length;
	UINT8   Param[PARAM_LEN];
		
}RFDATAPKT;

extern  volatile RFPKT      RF;
extern  volatile RFDATAPKT  g_RFRecData;	
extern  volatile RFDATAPKT  g_RFSendData;	    //RF send data package;

/*External Function States*/

UINT8 RFRecvPacket( void );
void RF_SendData( UINT8 *pSnd, UINT8 Len );
void FillDataPacket( UINT8 Cmd , UINT8 Type);

void SPI_Bank1_Write_Reg(UINT8 reg, UINT8 *pBuf);
void SPI_Bank1_Read_Reg(UINT8 reg, UINT8 *pBuf);
UINT8 Get_ChipID( void );
void Set_LowPowerMode( void );

UINT8 Read_RXPayload( UINT8* pBuff, UINT8 MaxLen );
void Write_TXPayload( UINT8 * pBuff, UINT8 Len );

UINT8 Check_Sum( UINT8 *pBuff , UINT8 Len );

#endif
