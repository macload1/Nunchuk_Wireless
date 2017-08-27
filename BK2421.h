#ifndef _RFINIT_H
#define _RFINIT_H

#include "GenericTypedefs.h"

#define TRUE            1
#define FALSE           0

#define UINT8           unsigned char
#define INT8            char
#define INT16           int
#define UINT8           unsigned char
#define UINT16          unsigned int
#define UINT32          unsigned long

#define MAX_PACKET_LEN  32// max value is 32


//************************FSK COMMAND and REGISTER****************************************//
// SPI(BK2421) commands
#define READ_REG                      0x00  // Define read command to register
#define WRITE_REG                     0x20  // Define write command to register
#define RD_RX_PLOAD                   0x61  // Define RX payload register address
#define WR_TX_PLOAD                   0xA0  // Define TX payload register address
#define FLUSH_TX                      0xE1  // Define flush TX register command
#define FLUSH_RX                      0xE2  // Define flush RX register command
#define REUSE_TX_PL                   0xE3  // Define reuse TX payload register command
#define W_TX_PAYLOAD_NOACK_CMD        0xb0
#define W_ACK_PAYLOAD_CMD             0xa8
#define ACTIVATE_CMD                  0x50
#define R_RX_PL_WID_CMD               0x60
#define NOP                           0xFF  // Define No Operation, might be used to read status register

// SPI(BK2421) registers(addresses)
#define CONFIG                        0x00  // 'Config' register address
#define EN_AA                         0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR                     0x02  // 'Enabled RX addresses' register address
#define SETUP_AW                      0x03  // 'Setup address width' register address
#define SETUP_RETR                    0x04  // 'Setup Auto. Retrans' register address
#define RF_CH                         0x05  // 'RF channel' register address
#define RF_SETUP                      0x06  // 'RF setup' register address
#define STATUS1                       0x07  // 'Status' register address
#define OBSERVE_TX                    0x08  // 'Observe TX' register address
#define CD                            0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0                    0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1                    0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2                    0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3                    0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4                    0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5                    0x0F  // 'RX address pipe5' register address
#define TX_ADDR                       0x10  // 'TX address' register address
#define RX_PW_P0                      0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1                      0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2                      0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3                      0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4                      0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5                      0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS                   0x17  // 'FIFO Status Register' register address
#define PAYLOAD_WIDTH                 0x20  // 'payload length of 256 bytes modes register address

//interrupt status
#define STATUS_RX_DR                  0x40
#define STATUS_TX_DS                  0x20
#define STATUS_MAX_RT                 0x10

#define STATUS_TX_FULL                0x01

//FIFO_STATUS
#define FIFO_STATUS_TX_REUSE          0x40
#define FIFO_STATUS_TX_FULL           0x20
#define FIFO_STATUS_TX_EMPTY          0x10

#define FIFO_STATUS_RX_FULL           0x02
#define FIFO_STATUS_RX_EMPTY          0x01

//Set RX address
#define RF_SET_RX_ADDR(addr) SPI_Write_Buf((WRITE_REG|RX_ADDR_P0), addr, 5)
//TX address
#define RF_SET_TX_ADDR(addr) SPI_Write_Buf((WRITE_REG|TX_ADDR), addr, 5)
//Set ACK
#define RF_SET_AUTO_ACK(enable) SPI_Write_Reg((WRITE_REG|EN_AA), enable)
//choice pipe
#define RF_SET_CHN(ch) SPI_Write_Reg((WRITE_REG|RF_CH), ch)
//Clear IRQ
#define RF_CLR_IRQ(x) SPI_Write_Reg(WRITE_REG|STATUS1, x)
//Read status register
#define RF_GET_RX_PL_LEN() SPI_Read_Reg(R_RX_PL_WID_CMD)
//Read result of carry detection
#define RF_GET_CD() SPI_Read_Reg(CD)
//Clear RX FIFO
#define RF_FLUSH_RX() SPI_Write_Reg(FLUSH_RX,0)
//Clear TX FIFO
#define RF_FLUSH_TX() SPI_Write_Reg(FLUSH_TX,0)



#define DYNPD                                  0x1c
#define FEATURE                                0x1d
#define BANK0_REG_LIST_CNT                     21
#define BANK0_REGACT_LIST_CNT                  2


#define     RFPKT_LEN       5

void BK2421_Initialize(void);

UINT8 SPI_Read_Reg(UINT8 reg);
void SPI_Read_Buf(UINT8 reg, UINT8 *pBuf, UINT8 bytes);

void SPI_Write_Reg(UINT8 reg, UINT8 value);
void SPI_Write_Buf(UINT8 reg, UINT8 *pBuf, UINT8 bytes);


void SwitchToTxMode(void);
void SwitchToRxMode(void);

void SPI_Bank1_Read_Reg(UINT8 reg, UINT8 *pBuf);
void SPI_Bank1_Write_Reg(UINT8 reg, UINT8 *pBuf);
void SwitchCFG(char _cfg);

UINT8 RF_ReadRxPayload( UINT8 *pbuf, UINT8 maxlen );

/*struct defination for RF transmit packet*/
typedef struct
{
	UINT8	sn;						// serial number for rf packet, increased each send ops
	UINT8	len;					// packet valid data length in param
	UINT8	sn_pkt;					// packet serial number, increased for each new data packet
	UINT8	param[PAYLOAD_WIDTH];	//
}RF_PKT;

#define  W_TX_PAYLOAD 		0XA0   	//Write Data to TX FIFO Command;
#define  W_ACK_PAYLOAD		0XA8	//Wirte ACK Data to FIFO;

#endif 
