#ifndef __RFCOM_H
#define __RFCOM_H

#include "stm8s.h"
#include "config.h"

typedef struct RF_Datagram
{
	uint8_t NodeID;
	uint8_t Sequence;
	uint8_t Control; //Msgtype = Control/64,Length = Control%16;
	uint8_t ItemID;	 //
	uint8_t Data[14];
} RF_Datagram_t;

typedef struct Group_Status
{
	bool Light_Enabled;
	bool Shutter_Enabled;
	bool LightsOn;
	uint8_t CW_Bright;
	uint8_t CC_Bright;
	uint8_t Shutter_Position;
	bool NightMode;
	bool MotionEnabled;
} Group_Status_t;

typedef struct RF_Params
{
	uint8_t NODEID;
	uint16_t MI_Remote_Address;
	bool Learn_Enabled;
	uint8_t Learn_Counter;
} RF_Params_t;

void RF_SetNodeID(uint8_t Node);
uint8_t RF_GetNodeID(void);
int8_t RF_RX_Handle(void);
int8_t RF_RX_Parse(uint8_t *RX_Buffer, int8_t Length);
bool RF_RX_MIRemote(uint8_t *RX_Buffer, int8_t Lenght);
void RF_RX_Command(uint8_t *RX_Buffer, int8_t Length);
bool RF_RX_MI_ChkAddress(uint16_t MI_Address, uint8_t MI_Command);
void RF_RX_Check_LT8900(void);
void RF_Send_Motion(uint8_t Group);



#endif
