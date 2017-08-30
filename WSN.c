/* 
 * File:   WSN.c
 * Author: macload1
 *
 * Created on 3 mars 2013, 15:34
 */
#include <p24Fxxxx.h>
#include <stdio.h>
#include <stdlib.h>

#include <outcompare.h>
#include <timer.h>

#include "RFInit.h"
#include "RF_SPI.h"
#include "TimeDelay.h"
#include "Bk_rxtx.h"

#define USE_AND_OR /* To enable AND_OR mask setting */

#define LED                 LATBbits.LATB7

#define RF_PKT_LEN		    25		//RF Packet length for transfer

/*struct defination for RF transmit packet*/
typedef struct
{
    UINT8   sn;     //serial number for rf packet, increased each send ops
    UINT8   len;    //packet valid data length in param
    UINT8   sn_pkt; //packet serial number, increased for each new data packet
    UINT8   param[RF_PKT_LEN];
}S_RF_PKT;

extern volatile RFDATAPKT g_RFSendData;		//RF send data package;

//void RF_Recv_Process( void );
//void RF_Send_Process( void );
//void RF_FillPacket( void );
//void RF_SendPacket( void );

void timerInit(void);
void outcompareInit(void);
void CNInit(void);

/* PIC24F32KA301 Configuration Bit Settings */


// FBS
#pragma config BWRP = OFF               // Boot Segment Write Protect (Disabled)
#pragma config BSS = OFF                // Boot segment Protect (No boot program flash segment)

// FGS
#pragma config GWRP = OFF               // General Segment Write Protect (General segment may be written)
#pragma config GSS0 = OFF               // General Segment Code Protect (No Protection)

// FOSCSEL
#pragma config FNOSC = FRCPLL           // Oscillator Select (Fast RC Oscillator with Postscaler and PLL Module (FRCDIV+PLL))
#pragma config SOSCSRC = DIG            // SOSC Source Type (Digital Mode for use with external source)
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
    CLKDIVbits.RCDIV = 0;   // Sets System to 32MHz CLK

    // Pins to digital
    ANSA = 0;
    ANSB = 0;
    
    // Pin Input/Output
    TRISBbits.TRISB7 = 0;   // Output compare 1 pin with LED
    LED = 0;                // LED switched off
    
    // timer 1 & 2 Initialisation
    timerInit();
    
    // Output Compare 0 Initialisation
    //outcompareInit();
    
    // Change Notification Initialisation
    CNInit();
    
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

    //RF_Send_Process();
    FillDataPacket( 0xCA , 0x02 );
    RF_SendData( (UINT8*)&g_RFSendData , g_RFSendData.Length );

    test = SPI_Read_Reg(RF_CH);

    test2 = Get_ChipID();
    sequence = 0;

    while(1)
    {
        DelayMs(1000);

        FillDataPacket( 0xCA , 0x02 );
        RF_SendData( (UINT8*)&g_RFSendData , g_RFSendData.Length );
    }

    return (EXIT_SUCCESS);
}



void timerInit(void)
{
    // Configure Timer1 to 1 KHz update rate
    ConfigIntTimer1(T1_INT_PRIOR_2 & T1_INT_ON);
    OpenTimer1(T1_ON &      //Timer1 ON
            T1_IDLE_CON &
            T1_GATE_OFF &   //Gated mode OFF
            T1_PS_1_64 &    //Prescale 1:64
            T1_SOURCE_INT, 250);
    
    // Configure Timer2 for OC module
    ConfigIntTimer2(T2_INT_OFF);
    OpenTimer2(T2_ON &      //Timer2 ON
            T2_IDLE_CON &
            T2_GATE_OFF &   //Gated mode OFF
            T2_PS_1_8 &     //Prescale 1:8
            T2_SOURCE_INT, 250);
    
    return;
}

void outcompareInit(void)
{
    OpenOC1(
         // Sets OC1CON1
         OC_IDLE_CON           &
         OC_TIMER2_SRC         &
         OC_FAULT0_IN_DISABLE  &
         OC_FAULT1_IN_DISABLE  &
         CMP_FAULT2_IN_DISABLE &
         OC_PWM_FAULT_CLEAR    &
         OC_TRIG_CLEAR_SYNC    &
         OC_PWM_EDGE_ALIGN,
         
         // Sets OC1CON2
         OC_FAULT_MODE_PWM_CYCLE &
         OC_PWM_FAULT_OUT_HIGH   &
         OC_FAULT_PIN_UNCHANGE   &
         OC_OUT_NOT_INVERT       &
         DELAY_OC_FALL_EDGE_00   &
         OC_CASCADE_DISABLE      &
         OC_SYNC_ENABLE          &
         OC_TRIGGER_TIMER        &
         OC_DIRN_OUTPUT          &
         OC_SYNC_TRIG_IN_CURR_OC,
         
         // Period
         1000,          // 1000 => 250 µs period
         // On-time
         500);

    return;
}

void CNInit(void)
{
//    InputChange_Clear_Intr_Status_Bit;			//Clear change notification interrupt status bit
//	ConfigIntCN(INT_ENABLE|INT_PRI_4);			//Enable Change notification interrupt with priority 4
//	EnableCN4;									//Enable change notification on pin CN4
    
    TRISAbits.TRISA4 = 1;   // Set pin CN0 as input
    CNEN1bits.CN0IE = 1;    // Enable Change Notification for pin CN0
    CNPU1bits.CN0PUE = 0;   // Disable pull-up resistor on pin CN0
    CNPD1bits.CN0PDE = 0;   // Disable pull-down resistor on pin CN0
    IFS1bits.CNIF = 0;      // Disable Change Notification Interrupt Flag
    IEC1bits.CNIE = 1;      // Enable Change Notification Interrupt
    IPC4bits.CNIP = 4;      // Interrupt priority of 4
    LED = PORTAbits.RA4;
    return;
}
