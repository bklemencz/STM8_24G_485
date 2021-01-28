/**
  ******************************************************************************
  * @file    Project/main.c
  * @author  MCD Application Team
  * @version V2.2.0
  * @date    30-September-2014
  * @brief   Main program body
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#ifndef __CSMC__
#define __CSMC__
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "lt8900.h"
#include "eeprom.h"
#include "delay.h"
#include "config.h"
#include "rfcomms.h"
#include "serialcomms.h"
#include "main.h"

/* Private defines -----------------------------------------------------------*/
uint8_t Main_State; //0-idle
uint16_t Main_Config;

uint8_t Conf_LED_Fade_Delay;
uint16_t Conf_LED_TurnOff_Delay_S;
uint8_t Conf_LED_Max_Brightness;
uint8_t Conf_LED_NightMode_Brightness;
extern Group_Status_t Group_Command[4];

extern RF_Params_t RF_Status;

//shutter control definitions
uint8_t Shutter_Config;
bool Shutter_Enabled[4];

// Serial Receive Buffer and counter
extern volatile uint8_t bufflen;
extern volatile uint8_t serialbuff[7];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void Init_GPIOS(void)
{

	//CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
	CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV);
	CLK->CKDIVR |= (uint8_t)0x00;
	
	GPIO_Init(RS485_TXEN_PORT,(GPIO_Pin_TypeDef) RS485_TXEN_PIN,GPIO_MODE_OUT_PP_HIGH_FAST);
	GPIO_WriteHigh(RS485_TXEN_PORT,(GPIO_Pin_TypeDef) RS485_TXEN_PIN);								//Enable 485 Tx
	UART1_DeInit();
	UART1_Init(RS485_BAUD,UART1_WORDLENGTH_8D,UART1_STOPBITS_1,UART1_PARITY_NO,UART1_SYNCMODE_CLOCK_DISABLE,UART1_MODE_TXRX_ENABLE);
	UART1_Cmd(ENABLE);

	
	LT8900_Init();
	
	//Timer_Init();
}

/// TODO Save LED Targets to EEPROM
void Save_Targets(void)
{
	
	EEPROM_ProgramByte(39, Group_Command[0].CW_Bright);
	EEPROM_ProgramByte(40, Group_Command[1].CW_Bright);
	EEPROM_ProgramByte(41, Group_Command[2].CW_Bright);
	EEPROM_ProgramByte(42, Group_Command[3].CW_Bright);

	EEPROM_ProgramByte(43, Group_Command[0].CC_Bright);
	EEPROM_ProgramByte(44, Group_Command[1].CC_Bright);
	EEPROM_ProgramByte(45, Group_Command[2].CC_Bright);
	EEPROM_ProgramByte(46, Group_Command[3].CC_Bright);
	
}

/// Setting Defult values before EEPROM read
void SetDefaults(void)
{
	
	RF_SetNodeID(RF_NODEID_DEF);
	if (LED_FADE_ENABLED) SetBit(Main_Config, MAINCONFIG_FADEE);
	if (RF_COM_USE)	SetBit(Main_Config,MAINCONFIG_USERF);
	if (RF_MILIGHT_REMOTE) SetBit(Main_Config, MAINCONFIG_MIREM);
	if (RF_MILIGHT_GATEW) SetBit(Main_Config, MAINCONFIG_MIGAT);
	if (START_LEDON) SetBit(Main_Config, MAINCONFIG_STRTON);
	if (LED_RX_FADE_DELAY_EN) SetBit(Main_Config, MAINCONFIG_FADEDELEN);
	if (RF_TX_PERIODIC_SEND)	SetBit(Main_Config, MAINCONFIG_RFPERTXEN);

	Conf_LED_Fade_Delay = LED_FADE_DELAY_MS;
	Conf_LED_TurnOff_Delay_S = LIGHTS_OFF_DELAY_S;
	Conf_LED_Max_Brightness = LIGHT_MAX_PERCENT;
	Conf_LED_NightMode_Brightness = LIGHT_NIGHT_PERCENT;
	//RF_RX_MI_NewRemote = FALSE;
}

void EE_Store_Config (void)
{
	//debug
	SetBit(Main_Config, MAINCONFIG_FADEDELEN);
	//
	EEPROM_Program2Byte(0, Main_Config);
	EEPROM_ProgramByte(2, Conf_LED_Fade_Delay);
	EEPROM_ProgramByte(3, Conf_LED_Max_Brightness);
	EEPROM_ProgramByte(4, Conf_LED_NightMode_Brightness);
	EEPROM_Program2Byte(5, Conf_LED_TurnOff_Delay_S);
	
	Save_Targets();				// 36 - 43 LED TARGETS
	if ( RF_Status.MI_Remote_Address !=0 ) EEPROM_Program2Byte(47, RF_Status.MI_Remote_Address);
	EEPROM_ProgramByte(49, RF_GetNodeID());
	EEPROM_ProgramByte(50,Shutter_Config);


}

void EE_Read_Config(void)
{
  if (EEPROM_Read2Byte(0)==0) EE_Store_Config();
	Main_Config = EEPROM_Read2Byte(0);
	Conf_LED_Fade_Delay = EEPROM_ReadByte(2);
	Conf_LED_Max_Brightness = EEPROM_ReadByte(3);
	Conf_LED_NightMode_Brightness = EEPROM_ReadByte(4);
	Conf_LED_TurnOff_Delay_S = EEPROM_Read2Byte(5);

	RF_Status.MI_Remote_Address = EEPROM_Read2Byte(47);
	RF_SetNodeID(EEPROM_ReadByte(49));

	Shutter_Config = EEPROM_ReadByte(50);
	if (bitRead(Shutter_Config,0) != 0)	Shutter_Enabled[0] = TRUE;
	if (bitRead(Shutter_Config,1) != 0)	Shutter_Enabled[1] = TRUE;
	if (bitRead(Shutter_Config,2) != 0)	Shutter_Enabled[2] = TRUE;
	if (bitRead(Shutter_Config,3) != 0)	Shutter_Enabled[3] = TRUE;
	
	//For Remote Control Command, when holding remote key it sends command without hold first.
	SetBit(Main_Config, MAINCONFIG_FADEDELEN);
	SetBit(Main_Config, MAINCONFIG_RFPERTXEN);

}

void StartUp(void)
{
	uint8_t i;
	Init_GPIOS();
	SetDefaults();
	EEPROM_Config();
	EE_Read_Config();

	enableInterrupts();
	// If using with a standard light switch, when turn on we want the lights
	// to go to the last saved brightness. Based on config bit.
	if (bitRead(Main_Config, MAINCONFIG_STRTON))
	{
		for (i=0;i<4;i++)
		{
			Group_Command[i].Light_Enabled = TRUE;
			Group_Command[i].CW_Bright = EEPROM_ReadByte(39+i);
			Group_Command[i].CC_Bright = EEPROM_ReadByte(43+i);
		}
	}
	_delay_ms(100);
	LT8900_softReset();
	LT8900_InitRegisters();                          // Init LT8900 registers
	LT8900_InitRegisters(); 
	LT8900_setChannel(DEFAULT_CHANNEL);                            // Set recieve channel
  	LT8900_startListening();												 // RX Mode
	Timer_Init();
	RF_Status.Learn_Enabled = TRUE;

}

void RF_Schedule(void)
{
	int8_t RX_Size;
	RX_Size = RF_RX_Handle();
	//RF_TX_Handle();
}

void Serial_Schedule(void)
{
	if (bufflen == 7)
	{
		if ((serialbuff[2] == 'C') && (serialbuff[3] == 'M'))
		{
			RF_Send_Motion(serialbuff[1]);
		}
		UART1_ITConfig(UART1_IT_RXNE_OR,ENABLE);
	}
}

void main(void)
{
	uint8_t i;
	StartUp();
	while (1)
	{
		RF_Schedule();
		Serial_Schedule();
	}
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
