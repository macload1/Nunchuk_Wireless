#include "RF_SPI.h"
#include "RFInit.h"
#include "Bk_rxtx.h"

#include "TimeDelay.h"

#define RF_CHANNEL           84

#define DATARATE_1M          0x07
#define DATARATE_2M          0x2F
#define DATARATE_250K        0x27


#define COMPATIBLE_BK2421_DEBUG
#define HIGH_SENSITIVITY_DEBUG
#define RATE_250K
//#define ENABLE_RSSI_DEBUG

//Bank1 register initialization value==================================================
//In the array Bank1_Reg0_13,all[] the register value is the byte reversed!!!
volatile UINT32 Bk2423_Bank1_Reg0_13[ ]={    
    ////REG0 to REG8°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠
    0xE2014B40,         //REG0
    0x00004BC0,         //REG1
    0x028CFCD0,         //REG2
    0x41390099,         //REG3       

    #ifdef HIGH_SENSITIVITY_DEBUG
    0x0B86BED9,         //REG4      high sensitivity mode       
    #else
    0x0B869ED9,         //REG4      normal sensitivity mode     
    #endif

    #ifdef  ENABLE_RSSI_DEBUG
    0xA67F023C          //REG5      Enable RSSI measurement
    #else
    0xA67F0624,         //REG5      Disable RSSI measurement
    #endif

    0x00000000,         //REG6
    0x00000000,         //REG7
    0x00000000,         //REG8
      
    ////REG9 to REG13°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠°≠
    0x00000000,         //REG9
    0x00000000,         //REG10
    0x00000000,         //REG11

    #ifdef COMPATIBLE_BK2421_DEBUG
    0x00127300,         //REG12      PLL Lcoking time 120us compatible with BK2421
    #else
    0x00127305,         //REG12      PLL Lcoking time 130us compatible with nRF24L01
    #endif

    0x36B48000,         //REG13      
};


UINT8 Bk2423_Bank1_Reg14[11]={ 
0x41,0x10,0x04,0x82,0x20,0x08,0x08,0xF2,0x7D,0xEF,0xFF       

};  
//Bank0 register initialization value
#define BK2423_REG_NUM      23     
UINT8 Bk2423_Bank0_Reg[][2]={
{ 0  , 0x0F },
{ 1  , 0x00 },              //Close All Pipe ACK
{ 2  , 0x01 },              //Enalbe Pipp0 RX Address
{ 3  , 0x03 },              //RX/TX address width 5B
{ 4  , 0x00 },              
{ 5  , 0x60 },            //Set channel
{ 6  , 0x0F },	            //≥ı ºªØ250K, 3DBM,High gain//0x0F or 0x2F:2Mbps; 0x07:1Mbps ; 0x27:250Kbps
{ 7  , 0x07 },
{ 8  , 0x00 },
{ 9  , 0x00 },
{ 12 , 0xc3 },
{ 13 , 0xc4 },
{ 14 , 0xc5 },
{ 15 , 0xc6 },
{ 17 , 0x20 },
{ 18 , 0x20 },
{ 19 , 0x20 },
{ 20 , 0x20 },
{ 21 , 0x20 },
{ 22 , 0x20 },
{ 23 , 0x00 },
{ 28 , 0x01 },               //Enable pipe 0, Dynamic payload length
{ 29 , 0x04 },               //EN_DPL= 1, EN_ACK_PAY = 0, EN_DYN_ACK = 0
};

UINT8 RX_Address[0x05]= { 0x2a, 0x2b, 0x2c, 0x2d, 0x02 };
//UINT8 RX_Address[0x05]= { 0x3a, 0x3b, 0x3c, 0x3d, 0x01 };
/******************************************************************************** 
Function:               void BANK0_Init( void ) 
Parameter: 
                        None
Return:
                        None
Description:
                        BANK0 register initialize operation
*********************************************************************************/
void Bk2423_BANK0_Init( void )
{
    UINT8  i  = 0x00 , k = 0x00;

    //Bank0 Register Configuration Operate============================================
    for( i = 0; i < 21; i++ )
	{
        SPI_Write_Reg( W_REGISTER | Bk2423_Bank0_Reg[i][0], Bk2423_Bank0_Reg[i][1] );
        SPI_Read_Reg( Bk2423_Bank0_Reg[i][0] );   
	}

    //Write RX/TX Address============================================================
	RF_SET_RX_ADDR( &RX_Address[0] );
	RF_SET_TX_ADDR( &RX_Address[0] );

 	k = SPI_Read_Reg( FEATURE );
	if( k == 0 )
	{
        SPI_Write_Reg( ACTIVATE,  0X73 );
	}

    //Bank0_Register Configuration Operat===========================================
	for( i = 21; i < 23; i++  )							
	{
        SPI_Write_Reg( W_REGISTER | Bk2423_Bank0_Reg[i][0], Bk2423_Bank0_Reg[i][1] );
        SPI_Read_Reg( Bk2423_Bank0_Reg[i][0] );
	}

}
/*********************************************************************************
Function:              	void BANK1_Init( void ) 
Parameter: 
                        None;
Return: 
                        None;
Description:
                        BANK1 regiter initialize operation,BANK1 Reg0 to Reg8 ,fisrt
Write High Byte ,then Write Low Byte;Reg9 to Reg14 First Write Low Byte,Then Write High 
Byte 
*********************************************************************************/
void Bk2423_BANK1_Init( void )
{
    UINT8 i= 0x00,j= 0x00,Buff[4] = {0};

    //Configuration Bank1 Register0 to Register8====================================  
    for( i = 0; i < 9; i++ )
    {
        for( j = 0; j < 4; j++ )
        {
            Buff[j] = (UINT8)(( Bk2423_Bank1_Reg0_13[i] >> ( 8 *(j) ) ) & 0xff );   
        }	
        SPI_Write_Buf( W_REGISTER | i, &(Buff[0]), 4 );
    }

    //Configuration Bank1 Register9 to Register13===================================
    for( i = 9; i < 14; i++ )
    {
        for( j = 0; j < 4; j++ )
        {
            Buff[j] = (UINT8)( Bk2423_Bank1_Reg0_13[i] >> 8 * (3-j) & 0xff );
        }
        SPI_Write_Buf( W_REGISTER | i, &(Buff[0]), 4 );
    }

    //Configuration  Bank1 Register 14==============================================
    SPI_Write_Buf( W_REGISTER | 0x0e,&(Bk2423_Bank1_Reg14[0]),11 );

    //toggle Reg4[25-26]============================================================
    for( i = 0; i < 4; i++ )
    {
        Buff[i] = (UINT8)(( Bk2423_Bank1_Reg0_13[4] >> 8*(i)) & 0xff );
    }
    Buff[0] |= 0x06;
    SPI_Write_Buf( W_REGISTER | 0X04, &(Buff[0]), 4 );
    
    Buff[0] &= 0XF9;
    SPI_Write_Buf( W_REGISTER | 0X04, &(Buff[0]), 4 );
    
}
/******************************************************************************** 
Function:              	void BK2423_Init( void ) 
Parameter: 
                        None
Return: 
                        None
Description:
                        Chip initialize and Configuration RX mode
*********************************************************************************/
void BK2423_Init( void )
{
    DelayMs( 100 );                  //hardware response time > 50ms;
    CLR_CE();                         //Closed RX mode or TX mode;

    //Bank0 initialize
    SwitchBANK( BANK_0 );
    Bk2423_BANK0_Init();

	DelayMs( 20 );

    //BANK1 initialize
    SwitchBANK( BANK_1 );
    Bk2423_BANK1_Init();

    DelayMs( 20 );
    SwitchBANK( BANK_0 );

    //Switch to RX MODE
    SwitchtoRXMode();

    //Clear RX FIFO and TX FIFO
    SPI_Write_Reg( FLUSH_RX, 0X00 );
    SPI_Write_Reg( FLUSH_TX, 0X00 );
}
/******************************************************************************** 
Function:              	void SwitchtoRXMode( void ) 
Parameter: 
                        None;
Return: 
                        None;
Description:
						Switch to RX mode operation
*********************************************************************************/
void SwitchtoRXMode( void )
{
    UINT8 Rt = 0;

	SPI_Write_Reg( FLUSH_RX, 0x00 );				//Clear RX FIFO;
	SPI_Write_Reg( W_REGISTER | STATUS, 0x70 );	    //clear RX_DR or TX_DS or MAX_RT interrupt flag	

	CLR_CE();

	Rt = SPI_Read_Reg( R_REGISTER | CONFIG );
	Rt |= ( 1 << 1 );  								//POWER UP mode
	Rt |= ( 1 << 0 );	   							//RX mode;
	SPI_Write_Reg( W_REGISTER | CONFIG, Rt );

	SET_CE();
}/******************************************************************************** 
Function:              	void SwitchtoTXMode( void ) 
Parameter: 
                        None;
Return: 
                        None;
Description:
					   	Switch to TX mode operation
*********************************************************************************/
void SwitchtoTXMode( void ) 
{
    UINT8 Rt = 0;
    SPI_Write_Reg( FLUSH_TX, 0X00 );    			//Clear TX FIFO;

	CLR_CE();
	Rt  = SPI_Read_Reg( R_REGISTER | CONFIG );
	Rt &= 0XFE;				   						//CLR PWR_RX Bit;
	SPI_Write_Reg( W_REGISTER | CONFIG, Rt );

//	SET_CE();  
}
/********************************************************************************
Function:             	void SwitchBANK( UINT8 pB  )
Parameter:
                       	pB	[IN]	Appoint switch to BANK0 or BANK1                                                                       
Return:                                                                            
                       	None;                                                                                                     );
Description: 
                        Switch to BANK0 or BANK1                                                                                   );
*********************************************************************************/
void SwitchBANK( UINT8 pB )
{
    UINT8 Rt    =   0x00;

    Rt  = SPI_Read_Reg( R_REGISTER | STATUS ); //Read STATUS_RBANK;
    Rt &= 0x80;

    if( ( Rt && !pB ) ||( !Rt && pB ))
    {
        SPI_Write_Reg( ACTIVATE, 0X53 );
    }

    SPI_Read_Reg( R_REGISTER | STATUS );    //Detect STATUS_RBANK = 0?
}
/******************************************************************************** 
Function:              	void Set_AirRate(UINT8 Rate ) 
Parameter: 
                        Rate	[IN]	
Return: 
                        None;
Description;            Bank0_Reg6[5,3]°≠°≠
                        LOW,HIGH: 00        1M     0x00
                        LOW,HIGH: 01        2M     0x08
                        LOW,HIGH: 10        250K   0x20
                        LOW,HIGH: 11        2M     0x28
*********************************************************************************/
void Set_AirRate(UINT8 Rate ) 
{
    UINT8   Rt_Value    =   0;

    Rt_Value     =   SPI_Read_Reg( R_REGISTER | RF_SETUP );
    Rt_Value   &=  ~( (1 << 3) | (1 << 5) );

    Rt_Value   |=   Rate;
    SPI_Write_Reg( W_REGISTER | RF_SETUP,Rt_Value );
    
    SPI_Read_Reg(  R_REGISTER | RF_SETUP );
}
/********************************************************************************
Function:           void BK2423_SwitchOutPower( UINT8 Power)
Parameter:
                    Power   [IN]  -15dBm; -7dBm; -2dBm; +3dBm
Return:
                    None
Description:
                    Bank1_REG4[20]  Bank0_REG6[2,1]
                        1               00          -15dBm        0x00
                        1               01          -7dBm         0x02
                        1               10          -2dBm         0x04
                        1               11          +3dBm         0x06
*********************************************************************************/
void Set_OutPower( UINT8 Power)
{
    UINT8 Rt_value = 0;

    Rt_value    =  SPI_Read_Reg( R_REGISTER | RF_SETUP );

    Rt_value   &=  0xF9;
    Rt_value   |=  Power;

    SPI_Write_Reg( W_REGISTER | RF_SETUP, Rt_value );
    SPI_Read_Reg(  R_REGISTER | RF_SETUP );
}
/******************************************************************************** 
Funcname:               void Set_PowerMode( UINT8 power_mode ) 
Parameter: 
                        PW  [IN] True = Power up mode,otherwise Power down mode;
Return: 
                        None
Description:
                        Switch to Power up mode or Power down mode
*********************************************************************************/
#define POWER_UP_MODE       0x01
#define POWER_DOWN_MODE     0x00
void Set_PowerMode( UINT8 power_mode )
{
    UINT8 temp_value   =   0x00;

    temp_value = SPI_Read_Reg( R_REGISTER | CONFIG );

    if( power_mode == POWER_UP_MODE )    
    {
        temp_value  |= ( 1 << 1 );      //Enable Power up mode
        SPI_Write_Reg( W_REGISTER | CONFIG, temp_value );

        SET_CE();
    }
    else
    {   
        CLR_CE();

        temp_value  &= ~( 1 << 1 );      //Enable Power Down mode
        SPI_Write_Reg( W_REGISTER | CONFIG, temp_value );
    }
}


/**************************************************************************
Function:           void Set_SenMode(UINT8 b_enable )
Parameter: 
                    b_enable    1:high sensitivity mode 
	                            0:normal sensitivity mode
Return:
                   
Description:
                    set sensitivity mode
***************************************************************************/           
void Set_SenMode(UINT8 b_enable )
{
	UINT8 j, WriteArr[4];

	for( j = 0x00;j < 0x04; j++ )
    {
        WriteArr[j] =  ( Bk2423_Bank1_Reg0_13[4]>>(8*(j) ) )&0xff;
    }

    if( b_enable )
    {
        WriteArr[1] =   WriteArr[1] | 0x20;  //Set REG4<21>
    }
    else
    {
        WriteArr[1] =   WriteArr[1] & 0xdf;  //Clear REG4<21>
    }

    //write REG4
	SPI_Bank1_Write_Reg( W_REGISTER|0x04 , WriteArr );
}
