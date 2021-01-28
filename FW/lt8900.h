#ifndef __lt8900_H
#define __lt8900_H

#include "stm8s.h"
#include "delay.h"
#include "config.h"

#define _USE_LT8920

/////// LT8900 REGISTER DEFINITIONS

#define REGISTER_READ       0x80  
#define REGISTER_WRITE      0x0  
#define REGISTER_MASK       0x7F  

#define R_CHANNEL           7
#define CHANNEL_RX_BIT      7
#define CHANNEL_TX_BIT      8
#define CHANNEL_MASK        0x7F  ///bin
#define DEFAULT_CHANNEL     74          //CW WW   sync: 0x0a0A 0x55AA   channels: 4, 39, 74

#define R_CURRENT           9
#define CURRENT_POWER_SHIFT 12
#define CURRENT_POWER_MASK  0xF000
#define CURRENT_GAIN_SHIFT  7
#define CURRENT_GAIN_MASK   0x0780

/* LT8910S only */
#define R_DATARATE          44
#define DATARATE_MASK       0x00FF
#define DATARATE_1MBPS      0x0100
#define DATARATE_250KBPS    0x0400
#define DATARATE_125KBPS    0x0800
#define DATARATE_62KBPS     0x1000

#define R_SYNCWORD1         36
#define R_SYNCWORD2         37
#define R_SYNCWORD3         38
#define R_SYNCWORD4         39

#define R_PACKETCONFIG      41
#define PACKETCONFIG_CRC_ON             0x8000
#define PACKETCONFIG_SCRAMBLE_ON        0x4000
#define PACKETCONFIG_PACK_LEN_ENABLE    0x2000
#define PACKETCONFIG_FW_TERM_TX         0x1000
#define PACKETCONFIG_AUTO_ACK           0x0800
#define PACKETCONFIG_PKT_FIFO_POLARITY  0x0400

#define R_STATUS            48
#define STATUS_CRC_BIT      15
#define STATUS_PKT_FLAG     6
#define STATUS_FIFO_FLAG    5
#define SYNCW_RECV          7


#define R_FIFO              50
#define R_FIFO_CONTROL      52

#define R_POWER_MODE        35
#define POWER_DOWN_ENABLE   0x8000
#define POWER_DOWN_DISABLE  0x7000



void LT8900_Init(void);
void LT8900_InitRegisters(void);
void LT8900_setChannel(uint8_t channel);
void LT8900_startListening(void);
bool LT8900_available(void);
bool LT8900_available_R(void);
bool LT8900_sendPacket(uint8_t *data, uint8_t packetSize);
int LT8900_read(uint8_t *buffer, uint8_t maxBuffer);
uint16_t LT8900_readRegister(uint8_t reg);
void LT8900_writeRegister(uint8_t reg, uint16_t data);
void LT8900_writeRegister2(uint8_t reg, uint8_t high, uint8_t low);
bool LT8900_setDataRate(uint8_t rate);
void LT8900_softReset(void);
//void LT8900_setSyncWord(uint32_t syncWord);

#endif