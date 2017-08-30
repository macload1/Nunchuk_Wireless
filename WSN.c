/* 
 * File:   WSN.c
 * Author: macload1
 *
 * Created on 3 mars 2013, 15:34
 */
#include <p24Fxxxx.h>
#include <stdio.h>
#include <stdlib.h>
#include "RFInit.h"
#include "RF_SPI.h"
#include "TimeDelay.h"
#include "Bk_rxtx.h"

#define USE_AND_OR /* To enable AND_OR mask setting */




#define RF_PKT_LEN		    25		//RF Packet length for transfer

/*struct defination for RF transmit packet*/
typedef struct
{
    UINT8   sn;     //serial number for rf packet, increased each send ops
    UINT8   len;    //packet valid data length in param
    UINT8   sn_pkt; //packet serial number, increased for each new data packet
    UINT8   param[RF_PKT_LEN];
}S_RF_PKT;

//void RF_Recv_Process( void );
void RF_Send_Process( void );
void RF_FillPacket( void );
void RF_SendPacket( void );


/* PIC24F32KA301 Configuration Bit Settings */


// FBS
#pragma config BWRP = OFF               // Boot Segment Write Protect (Disabled)
#pragma config BSS = OFF                // Boot segment Protect (No boot program flash segment)

// FGS
#pragma config GWRP = OFF               // General Segment Write Protect (General segment may be written)
#pragma config GSS0 = OFF               // General Segment Code Protect (No Protection)

// FOSCSEL
#pragma config FNOSC = FRCPLL           // Oscillator Select (Fast RC Oscillator with Postscaler and PLL Module (FRCDIV+PLL))
#pragma config SOSCSRC = ANA            // SOSC Source Type (Analog Mode for use with crystal)
#pragma config LPRCSEL = HP             // LPRC Oscillator Power and Accuracy (High Power, High Accuracy Mode)
#pragma config IESO = ON                // Internal External Switch Over bit (Internal External Switchover mode enabled (Two-speed Start-up enabled))

// FOSC
#pragma config POSCMOD = NONE           // Primary Oscillator Configuration bits (Primary oscillator disabled)
#pragma config OSCIOFNC = OFF           // CLKO Enable Configuration bit (CLKO output disabled)
#pragma config POSCFREQ = HS            // Primary Oscillator Frequency Range Configuration bits (Primary oscillator/external clock input frequency greater than 8MHz)
#pragma config SOSCSEL = SOSCLP         // SOSC Power Selection Configuration bits (Secondary Oscillator configured for high-power operation)
#pragma config FCKSM = CSECME           // Clock Switching and Monitor Selection (Both Clock Switching and Fail-safe Clock Monitor are enabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscale Select bits (1:32768)
#pragma config FWPSA = PR128            // WDT Prescaler bit (WDT prescaler ratio of 1:128)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bits (WDT disabled in hardware; SWDTEN bit disabled)
#pragma config WINDIS = OFF             // Windowed Watchdog Timer Disable bit (Standard WDT selected(windowed WDT disabled))

// FPOR
#pragma config BOREN = BOR3             // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware, SBOREN bit disabled)
#pragma config LVRCFG = OFF             // (Low Voltage regulator is not available)
#pragma config PWRTEN = ON              // Power-up Timer Enable bit (PWRT enabled)
#pragma config I2C1SEL = PRI            // Alternate I2C1 Pin Mapping bit (Use Default SCL1/SDA1 Pins For I2C1)
//#pragma config BORV = V18               // Brown-out Reset Voltage bits (Brown-out Reset set to lowest voltage (1.8V))
#pragma config MCLRE = ON               // MCLR Pin Enable bit (RA5 input pin disabled,MCLR pin enabled)

// FICD
#pragma config ICS = PGx1               // ICD Pin Placement Select bits (EMUC/EMUD share PGC1/PGD1)

// FDS
#pragma config DSWDTPS = DSWDTPSF       // Deep Sleep Watchdog Timer Postscale Select bits (1:2,147,483,648 (25.7 Days))
#pragma config DSWDTOSC = LPRC          // DSWDT Reference Clock Select bit (DSWDT uses Low Power RC Oscillator (LPRC))
#pragma config DSBOREN = ON             // Deep Sleep Zero-Power BOR Enable bit (Deep Sleep BOR enabled in Deep Sleep)
#pragma config DSWDTEN = OFF            // Deep Sleep Watchdog Timer Enable bit (DSWDT disabled)


/* Global Variables */
static volatile S_RF_PKT g_rf_packet;      //packet buffer for rf transmit

char data_string[30];

long temperatur;
INT32 pressure;
UINT32 press;

volatile unsigned char test;
volatile unsigned char test2;
unsigned char sequence;

/*
 * Receiver code
 */
int main(int argc, char** argv)
{
    unsigned int boucle = 5000;
    while(boucle)
    {
        boucle--;
    }

    // Pins to digital
    ANSA = 0;
    ANSB = 0;

    // Place the new oscillator selection in W0
    // OSCCONH (high byte) Unlock Sequence
    asm volatile ("mov #OSCCONH,W1");
    asm volatile ("mov #0x78,W2");
    asm volatile ("mov #0x9A,W3");
    asm volatile ("mov.b W2,[W1]");
    asm volatile ("mov.b W3,[W1]");
    // Set new oscillator selection
    asm volatile ("mov.b WREG,OSCCONH");
    // OSCCONL (low byte) unlock sequence
    asm volatile ("mov #OSCCONL,W1");
    asm volatile ("mov.b #0x01,W0");
    asm volatile ("mov #0x46,W2");
    asm volatile ("mov #0x57,W3");
    asm volatile ("mov.b W2,[W1]");
    asm volatile ("mov.b W3,[W1]");
    // Start oscillator switch operation
    asm volatile ("mov.b W0,[W1]");

    sprintf(data_string, "Hello World!");


    // Initialisation
    RF_SPI_Init();

    // BK2421 Initialisation
    BK2423_Init();
    
//    SET_CHNNL( 78 );                    //Channel         =   78
//    Set_OutPower( BK2423_P3 );          //Output Power    =   3dBm
//    Set_AirRate( BK2423_RATE_250K );    //AirRate         =   250Kbps

    g_rf_packet.len = 20;
    g_rf_packet.sn = 7;
    g_rf_packet.sn_pkt = 15;

    RF_Send_Process();

    test = SPI_Read_Reg(RF_CH);

    test2 = Get_ChipID();
    sequence = 0;

    while(1)
    {

        DelayMs(1000);

        sequence++;
        RF_Send_Process();

    }

    return (EXIT_SUCCESS);
}


/*****************************************************************************************
Function:       void RF_Recv_Process( void )
Parameter:
                None
Return:
                None
Description:
                Read rf receive data out of the RF fifo, once the IRQ valid
                then fill it to usb send fifo
*****************************************************************************************/
//void RF_Recv_Process( void )
//{
//    UINT8 sta;
//    UINT8 rlen;
//    UINT8 *pPkt;
//
//    if( g_rf.irqvalid )
//    {
//	g_rf.irqvalid = FALSE;
//
//        sta = RF_GET_STATUS();      //Get the RF status
//
//        if( sta & STATUS_RX_DR )    //Receive OK?
//        {
//            //Readout the received data from RX FIFO
//            rlen = RF_ReadRxPayload( (UINT8 *)&g_rf_packet, sizeof(S_RF_PKT) );
//			RF_FLUSH_RX();
//
//            //Is a resend packet?
//            if( g_rf_packet.sn_pkt != g_rf.sn_recv )
//            {
//                //records the packet sn
//                g_rf.sn_recv = g_rf_packet.sn_pkt;
//
//                //Limit the data length of received packet
//                rlen = g_rf_packet.len;
//                if( rlen > RF_PKT_LEN )
//                {
//                    rlen = RF_PKT_LEN;
//                }
//
//                //fill the data of received packet to usb send fifo
//                pPkt = (UINT8 *)g_rf_packet.param;
//                while( rlen-- )
//                {
////                    g_usb_fifo[g_usb.pos_w++] = *pPkt++;
////                    if( g_usb.pos_w >= FIFO_LEN_USB )
////                    {
////                        g_usb.pos_w = 0;
////                    }
////                    g_usb.length++;
//                }
//            }
//        }
//        if( sta & STATUS_MAX_RT )  //Send fail?
//        {
//            RF_FLUSH_TX();  //Flush the TX FIFO
//        }
//
//        RF_CLR_IRQ( sta );  //Clear the IRQ flag
//    }
//}


/*******************************************************************************
Function:   void RF_Send_Process( void )
Parameter:
            None
Return:
            None
Description:
            Firstly, read data from I2C buffer and fill into the RF send packet;
            Secondly, send out the RF packet with retransmit till success.
*******************************************************************************/
void RF_Send_Process( void )
{
    //Fill a new packet data
    RF_FillPacket();
    // Send a new packet data
    RF_SendPacket();
}



/*****************************************************************************************
Function:       void RF_FillPacket( void )
Parameter:
                None
Return:
                None
Description:
                Read one packet from I2C data fifo, and prepare for send
*****************************************************************************************/
void RF_FillPacket( void )
{
    UINT8 *pPkt = (UINT8 *)g_rf_packet.param;
    //Limit the send length of current packet
    UINT8 sendlen = 21;

    //fill sn
    g_rf_packet.sn = g_rf_packet.sn++;
    g_rf_packet.sn_pkt = g_rf_packet.sn_pkt++;
    g_rf_packet.len = sendlen + 1;

    //Copy data from rf send fifo
    while( sendlen-- )
    {
        *pPkt++ = (unsigned char) data_string[20-sendlen];
    }
}


/*****************************************************************************************
Function:       void RF_SendPacket( void )
Parameter:
                None
Return:
                None
Description:
                Send out a packet via RF
                And clear the send packet length once success
*****************************************************************************************/
void RF_SendPacket( void )
{
    //SwitchToTxMode();   //Switch RF to TX mode
    SwitchtoTXMode();   //Switch RF to TX mode

    CLR_CE();
    /* Writes data to TX FIFO */
    SPI_Write_Buf(W_TX_PAYLOAD, (UINT8 *)&g_rf_packet, sizeof(S_RF_PKT));
    SET_CE();
    Delay10us(10);   //Wait for Time > 10us
    CLR_CE();

//    //Wait for send over
//    while( 1 )
//    {
//        if( g_rf.irqvalid )
//            {
//            sta = SPI_Read_Reg( STATUS );   // read register STATUS's value
//
//            if( sta & STATUS_MAX_RT )   //if send fail
//            {
//                RF_FLUSH_TX();
//                outflag = TRUE;
//            }
//            if( sta & STATUS_TX_DS )    //TX IRQ?
//            {
//                outflag = TRUE;
//                g_rf.packetlen = 0;     //Now send success, clear the send packet length
//            }
//
//            RF_CLR_IRQ( sta );          // clear RX_DR or TX_DS or MAX_RT interrupt flag
//            g_rf.irqvalid = FALSE;
//
//            if( outflag )
//            {
//                break;
//            }
//        }
//        else if( g_rf.sendtick >= 2 )  //if timeout
//        {
//            RF_FLUSH_TX();
//            break;
//        }
//    }

    //SwitchToRxMode();   //Switch to RX mode
    SwitchtoRXMode();   //Switch to RX mode
}


