#ifndef __SECOM_H
#define __SECOM_H

#include "stm8s.h"

void Serial_Send_PWM_Onoff(uint8_t ID, bool Onoff);
void Serial_Send_PWM_MotionDet(uint8_t ID);
void Serial_Send_PWM_MotionEN(uint8_t ID, bool MotionEN);
void Serial_Send_PWM_NightEN(uint8_t ID, bool NightEN);
void Serial_Send_PWM_Bright(uint8_t ID, uint8_t Ch1, uint8_t Ch2);
void Serial_Send_PWM_ChBright(uint8_t ID, uint8_t Ch, uint8_t Val);
void Serial_Send_PWM_OnTime(uint8_t ID, uint8_t Val);
void Serial_Send_PWM_NightBright(uint8_t ID, uint8_t Val);          //Set Night mode PWM level
void Serial_Send_PWM_MaxBright(uint8_t ID, uint8_t Val);
void Serial_Send_PWM_FadeTime(uint8_t ID, uint8_t Val);
void Serial_Send_Raw(uint8_t Length, uint8_t *ToSend, uint8_t DelayTime);
void Serial_Send_Shutter_Start(uint8_t ID);
void Serial_Send_Shutter_GoUp(uint8_t ID);
void Serial_Send_Shutter_GoDown(uint8_t ID);
void Serial_Send_Shutter_Position(uint8_t ID, uint8_t Position);

#endif
