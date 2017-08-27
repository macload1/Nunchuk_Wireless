#ifndef _BK2423_INIT_H
#define _BK2423_INIT_H

#include "GenericTypeDefs.h"
//************************FSK COMMAND and REGISTER****************************************//
// SPI(BK2423) commands
#define R_REGISTER              0x00  // Define read command to register
#define W_REGISTER              0x20  // Define write command to register
                                      
/*
#define R_RX_PL_WID_CMD         0x60                                       
#define RD_RX_PLOAD             0x61  // Define RX payload register address
#define WR_TX_PLOAD             0xA0  // Define TX payload register address
#define W_ACK_PAYLOAD_CMD       0xa8                                       
*/

#define	R_RX_PLY_WID		    0x60	//Read Length Command;
#define R_RX_PAYLOAD		    0x61	//Read Byte From RX FIFO Command;
#define W_TX_PAYLOAD 		    0xA0   	//Write Data to TX FIFO Command;
#define W_ACK_PAYLOAD		    0XA8	//Wirte ACK Data to FIFO;

#define FLUSH_TX                0xE1  // Define flush TX register command
#define FLUSH_RX                0xE2  // Define flush RX register command
#define REUSE_TX_PL             0xE3  // Define reuse TX payload register command

#define W_TX_PAYLOAD_NOACK_CMD	0xb0
#define ACTIVATE		        0x50
#define NOP                     0xFF  // Define No Operation, might be used to read status register



// SPI(BK2423) registers(addresses)
#define CONFIG                  0x00  // 'Config' register address
#define EN_AA                   0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR               0x02  // 'Enabled RX addresses' register address
#define SETUP_AW                0x03  // 'Setup address width' register address
#define SETUP_RETR              0x04  // 'Setup Auto. Retrans' register address
#define RF_CH                   0x05  // 'RF channel' register address
#define RF_SETUP                0x06  // 'RF setup' register address
#define STATUS                  0x07  // 'Status' register address
#define OBSERVE_TX              0x08  // 'Observe TX' register address
#define CD                      0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0              0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1              0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2              0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3              0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4              0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5              0x0F  // 'RX address pipe5' register address
#define TX_ADDR                 0x10  // 'TX address' register address
#define RX_PW_P0                0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1                0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2                0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3                0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4                0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5                0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS             0x17  // 'FIFO Status Register' register address
#define PAYLOAD_WIDTH           0x1f  // 'payload length of 256 bytes modes register address
#define DYNPD			        0X1C  //Enable Dynamic Payload Length;
#define	FEATURE		            0X1D  //Feature Register Address Setting

//FIFO Register interrupt status
#define STATUS_RX_DR            0x40
#define STATUS_TX_DS            0x20
#define STATUS_MAX_RT           0x10

#define STATUS_TX_FULL          0x01

//FIFO_STATUS
#define FIFO_STATUS_TX_REUSE    0x40
#define FIFO_STATUS_TX_FULL     0x20
#define FIFO_STATUS_TX_EMPTY    0x10

#define FIFO_STATUS_RX_FULL     0x02
#define FIFO_STATUS_RX_EMPTY    0x01

//////////////////////////////////////////////////////////////////////////////////
//Setting RX address
#define RF_SET_RX_ADDR(addr) { SPI_Write_Buf(( W_REGISTER | RX_ADDR_P0), addr, 5); }   
//Setting TX address
#define RF_SET_TX_ADDR(addr) { SPI_Write_Buf(( W_REGISTER | TX_ADDR   ), addr, 5); } 
//Setting Channle
#define SET_CHNNL( x )     { SPI_Write_Reg( W_REGISTER | RF_CH, x ); }



/*External Function State*/
void BK2423_Init( void );

void SwitchtoRXMode( void );
void SwitchtoTXMode( void );
void SwitchBANK( UINT8 pB );

void Set_AirRate(UINT8 Rate );
void Set_OutPower( UINT8 Power);

void Set_SenMode(UINT8 b_enable );
void Set_PowerMode( UINT8 out_power );


#endif
