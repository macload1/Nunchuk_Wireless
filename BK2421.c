////////////////////////////////////////////////////////////////////////////////////////
/// Includes'function; 
/// Bk2421_Init();  BANK0_Init();  BANK1_Init();  SwitchtoRXMode(); SwtichtoTXMode()
/// SwitchBANK();   Set_AirRate();  Set_OutPower(); Set_PWMode();
////////////////////////////////////////////////////////////////////////////////////////
#include "GenericTypedefs.h"
#include "RF_SPI.h"
#include "BK2421.h"
#include "TimeDelay.h"

volatile UINT32 Bank1_Reg0_13[]={
0xE2014B40,
0x00004BC0,
0x028CFCD0,
0x41390099,
0x0B869ED9,     //Change REG4[29:27] from 00 to 11
//0x21869ED9,   //For single carrier mode
0xA67F0624,     //Disable RSSI measurement
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00127300,
0x36B48000,
};

//Reg14,    Ramp curve
volatile UINT8 Bank1_Reg14[]=
{
0x41,0x20,0x08,0x04,0x81,0x20,0xCF,0xF7,0xFE,0xFF,0xFF
};


/*Bank0 register initialization value*/
volatile UINT8 Bank0_Reg[BANK0_REG_LIST_CNT][2]={
        {CONFIG,0x0F},                  //Power up, PRX
        {EN_AA,0x01},                   //Enable pipe0 auto ACK
        {EN_RXADDR,0x01},               //Enable data pipe 0
        {SETUP_AW,0x03},                //Address width = 5Bytes
        {SETUP_RETR,0x06},              //Retransmit, ARD = 250us, ARC = 5
        {RF_CH,0x60},                   //channel = 60
        {RF_SETUP,0x1F},                //air rate = 2Mbps,high gain,output power = 5dBm;
        {STATUS1,0x70},
        {OBSERVE_TX,0x00},
        {CD,0x00},
        {RX_ADDR_P2,0xc3},
        {RX_ADDR_P3,0xc4},
        {RX_ADDR_P4,0xc5},
        {RX_ADDR_P5,0xc6},
        {RX_PW_P0,0x20},                //RX Payload Length = 32
        {RX_PW_P1,0x20},
        {RX_PW_P2,0x20},
        {RX_PW_P3,0x20},
        {RX_PW_P4,0x20},
        {RX_PW_P5,0x20},
        {FIFO_STATUS,0x11}
};


volatile UINT8 Bank0_RegAct[BANK0_REGACT_LIST_CNT][2] = {
        {DYNPD,0x01},                 //Enable pipe 0, Dynamic payload length
        {FEATURE,0x04}                //EN_DPL= 1, EN_ACK_PAY = 0, EN_DYN_ACK = 0
};

//ADDRESS
UINT8 RX_Address[5] = { 0x2a, 0x2b, 0x2c, 0x2d, 0x02 };
UINT8 TX_Address[5] = { 0x2a, 0x2b, 0x2c, 0x2d, 0x02 };



/**************************************************
Function: SwitchToRxMode();
Description:
        switch to Rx mode
**************************************************/
void SwitchToRxMode(void)
{
	UINT8 value;
	
	SPI_Write_Reg(FLUSH_RX,0);//flush Rx

	value=SPI_Read_Reg(STATUS1);        // read register STATUS's value
	SPI_Write_Reg(WRITE_REG|STATUS1,value);// clear RX_DR or TX_DS or MAX_RT interrupt flag

	CLR_CE();
	
	value=SPI_Read_Reg(CONFIG);        // read register CONFIG's value
	
	value=value|0x01;//set bit 1
	SPI_Write_Reg(WRITE_REG | CONFIG, value); // Set PWR_UP bit, enable CRC(2 length) & Prim:RX. RX_DR enabled..

	SET_CE();
}

/**************************************************
Function: SwitchToTxMode();
Description:
        switch to Tx mode
**************************************************/
void SwitchToTxMode(void)
{
	UINT8 value;
	SPI_Write_Reg(FLUSH_TX,0);	//flush Tx

	CLR_CE();
	value = SPI_Read_Reg(CONFIG);	// read register CONFIG's value
	
	value = value & 0xfe;		//set bit 1
	SPI_Write_Reg(WRITE_REG | CONFIG, value);	// Set PWR_UP bit, enable CRC(2 length) & Prim:RX. RX_DR enabled.
	
	SET_CE();
}

/**************************************************
Function: SwitchCFG();

Description:
         access switch between Bank1 and Bank0

Parameter:
        _cfg      1:register bank1
                  0:register bank0
Return:
     None
**************************************************/
void SwitchCFG(char _cfg)//1:Bank1 0:Bank0
{
	UINT8 Tmp;
	
	Tmp = SPI_Read_Reg(7);
	Tmp = Tmp & 0x80;
	
	if( ( (Tmp) && (_cfg==0) )||( ((Tmp)==0) && (_cfg) ) )
	{
		SPI_Write_Reg(ACTIVATE_CMD, 0x53);
	}
}

/**************************************************
Function: SetChannelNum();
Description:
        set channel number

**************************************************/
void SetChannelNum(UINT8 ch)
{
	SPI_Write_Reg((UINT8)(WRITE_REG|5),(UINT8)(ch));
}



///////////////////////////////////////////////////////////////////////////////
//                  BK2421 initialization                                    //
///////////////////////////////////////////////////////////////////////////////
/**************************************************
Function: BK2421_Initialize();

Description:
        register initialization
**************************************************/
void BK2421_Initialize()
{
	signed char i,j;
	UINT8 rData;
	UINT8 WriteArr[4];
	
	DelayMs(100);	//delay more than 50ms.
	
	SwitchCFG(0);
	
	//********************Write Bank0 register******************
	for( i = (BANK0_REG_LIST_CNT - 1); i >= 0; i-- )
	{
		SPI_Write_Reg( (WRITE_REG | Bank0_Reg[i][0]), Bank0_Reg[i][1] );
		rData = SPI_Read_Reg( Bank0_Reg[i][0] );
	}

	//reg 10 - Rx0 addr
	RF_SET_RX_ADDR( (UINT8 *)RX_Address );

	//REG 16 - TX addr
	RF_SET_TX_ADDR( (UINT8 *)TX_Address );

	i = SPI_Read_Reg( FEATURE );

	if( i == 0 ) // i!=0 showed that chip has been actived.so do not active again.
	{
		SPI_Write_Reg( ACTIVATE_CMD, 0x73 );// Active
	}

	for( i = (BANK0_REGACT_LIST_CNT - 1); i >= 0; i-- )
	{
		SPI_Write_Reg( (WRITE_REG | Bank0_RegAct[i][0]), Bank0_RegAct[i][1] );

		SPI_Read_Reg( (Bank0_RegAct[i][0]) );
	}


	//********************Write Bank1 register******************
	SwitchCFG(1);

	for( i = 0; i <= 8; i++ )//reverse
	{
		for( j = 0; j < 4; j++ )
		{
			WriteArr[ j ] = ( Bank1_Reg0_13[i] >> ( 8 * (j) ) ) & 0xff;
		}

		SPI_Write_Buf( (WRITE_REG|i), &(WriteArr[0]), 4 );
	}
    
	for( i = 9; i <= 13; i++ )//reverse
	{
		for( j = 0; j < 4; j++ )
		{
			WriteArr[ j ] = ( Bank1_Reg0_13[i] >> ( 8 * (3-j) ) ) & 0xff;
		}

		SPI_Write_Buf( (WRITE_REG|i), &(WriteArr[0]), 4 );
	}


	SPI_Write_Buf( ( WRITE_REG|14), (UINT8 *)&(Bank1_Reg14[0]), 11 );

	//toggle REG4<25,26>
	for( j = 0; j < 4; j++ )
	{
		WriteArr[j] = ( Bank1_Reg0_13[4] >> ( 8 * (j) ) ) & 0xff;
	}

	WriteArr[0] = WriteArr[0] | 0x06;
	SPI_Write_Buf( (WRITE_REG | 4), &(WriteArr[0]), 4 );

	WriteArr[0] = WriteArr[0] & 0xf9;
	SPI_Write_Buf( (WRITE_REG | 4 ), &(WriteArr[0] ), 4 );

	DelayMs( 10 );

	//********************switch back to Bank0 register access******************
	SwitchCFG( 0 );

	SwitchToRxMode();//switch to RX mode

	RF_FLUSH_RX();
	RF_FLUSH_TX();
}

/*****************************************************************************************
Function:       UINT8 RF_ReadRxPayload( UINT8 *pbuf, UINT8 maxlen )
Parameter: 
                pbuf    [OUT]    receive data buffer
                len     [IN]    data buffer max length
Return: 
                The count wrote to buffer 
Description:
	            Read received data from RX FIFO
*****************************************************************************************/
UINT8 RF_ReadRxPayload( UINT8 *pbuf, UINT8 maxlen )
{
    UINT8 i = 0;
	UINT8 len = RF_GET_RX_PL_LEN();	//Get Top of fifo packet length

	if( len > maxlen )
	{
		len = maxlen;
	}

    for ( i = 0; i < maxlen; i++ )//Clear buffer
    {
        pbuf[ i ] = 0;
    }

	SPI_Read_Buf(RD_RX_PLOAD, pbuf, len);// read receive payload from RX_FIFO buffer
	
	return len;
}

