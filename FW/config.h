/*
Dec 10 2020 V2.11 
CHANGE
 - Sequence check on RF commands to filter multiples
 - Lt8900 reset if corrupted
 - Shutter working
 - Broadcast ID at commands
 - Rx Mode immedeiately after send

Dec 15 2020 V2.12 
 - Command 10 to set periodic message timer
 - Create command spreadsheet
 - Multiple message sending
 - Commands update as in spreadsheet

 TODO
 
 - Turn off when hold
 - Motion enabled on single press
 - Add Temp calib 
 - Add light level calib

*/
#ifndef __CONFIG_H
#define __CONFIG_H

#include "stm8s.h"

#include "delay.h"
#include "math.h"



#define RF_COM_USE              1
#define RF_NODEID_DEF           0x12
#define RF_MILIGHT_REMOTE       1
#define RF_MILIGHT_GATEW        1
#define RF_MILIGHT_HOLD_STEP    5
#define RF_TX_PERIODIC_SEND     1
#define RF_TX_PERIODIC_TIME     30000
#define RF_TX_REPEAT            4

#define LIGHTS_BY_MOTION        1
#define START_LEDON             1
#define LIGHTS_OFF_DELAY_S      10
#define LIGHT_MAX_PERCENT       99
#define LIGHT_NIGHT_PERCENT     10
#define RF_MI_LEARN_WINDOW_MS   60000
#define LED_FADE_ENABLED        1
#define LED_FADE_DELAY_MS       10
#define LED_RX_FADE_DELAY_EN    1
#define LED_DELAY_AFTER_RX      350
#define LED_FLASH_BRIGHTNESS    30

#define USE_485_COM             1
#define RS485_BAUD              19200
#define RS485_CHAR_DELAY        0
#define RS485_MESSAGE_DELAY     60
#define RS485_MESSAGE_TIMOUT_MS 20


#define MAINCONFIG_USERF        0       //USE RF FUCTIONS
#define MAINCONFIG_MIREM        1       //MI REMOTE RECEIVE FUNCTIONS
#define MAINCONFIG_MIGAT        2       //MI REMOTE GATEWAY FUNCTIONS
#define MAINCONFIG_MOVL         3
#define MAINCONFIG_FADEE        4       //LIGHT FADING ENABLED
#define MAINCONFIG_STRTON       5       //START ON WHEN STARTUP
#define MAINCONFIG_FADEDELEN    6       //FADE DELAY for remote hold
#define MAINCONFIG_RFPERTXEN    7       //PERIODIC MESSAGES ENABLED

#define RF_TX_ARQ               5
#define RF_TX_ARS               4

#define RF_RX_MOT_ALM           16
#define RF_TX_ALM_ITEM          64
#define RF_TX_PER_ITEM          65



#define LT8900_PKT_GPIO_PORT    (GPIOC)
#define LT8900_PKT_PIN          (GPIO_PIN_4)
#define LT8900_NCS_GPIO_PORT	(GPIOA)
#define LT8900_NCS_PIN          (GPIO_PIN_3)
#define SPI_PRESC               SPI_BAUDRATEPRESCALER_16

#define RS485_TXEN_PORT         (GPIOD)
#define RS485_TXEN_PIN          (GPIO_PIN_4)


#define TIM4_PERIOD             124
#define TIM4_PRESC              TIM4_PRESCALER_128




#define _BV(bit) (1 << (bit))
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

#endif
