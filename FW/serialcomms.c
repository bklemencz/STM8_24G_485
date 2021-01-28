#include "config.h"
#include "serialcomms.h"

uint8_t SBuffer[7];

void Serial_Send_PWM_Onoff(uint8_t ID, bool Onoff)
{
    SBuffer[0]=('P');    
    SBuffer[1]=(ID);
    SBuffer[2]=('C');
    if (Onoff)  SBuffer[3]=('O');
    else    SBuffer[3]=('F');
    SBuffer[4]=('0');
    SBuffer[5]=('0');
    SBuffer[6]=('\r');
    Serial_Send_Raw(7,SBuffer,RS485_CHAR_DELAY);
	
}

void Serial_Send_PWM_MotionDet(uint8_t ID)
{
    SBuffer[0]=('P');
    SBuffer[1]=(ID);
    SBuffer[2]=('C');
    SBuffer[3]=('M');
    SBuffer[4]=('0');
    SBuffer[5]=('0');
    SBuffer[6]=('\r');
    Serial_Send_Raw(7,SBuffer,RS485_CHAR_DELAY);
}

void Serial_Send_PWM_MotionEN(uint8_t ID, bool MotionEN)
{
    SBuffer[0]=('P');
    SBuffer[1]=(ID);
    SBuffer[2]=('S');
    SBuffer[3]=('E');
    if (MotionEN)   SBuffer[4]=('1');
    else    SBuffer[4]=('0');
    SBuffer[5]=('0');
    SBuffer[6]=('\r');
    Serial_Send_Raw(7,SBuffer,RS485_CHAR_DELAY);
}

void Serial_Send_PWM_NightEN(uint8_t ID, bool NightEN)
{
    SBuffer[0]=('P');
    SBuffer[1]=(ID);
    SBuffer[2]=('C');
    SBuffer[3]=('N');
    if (NightEN)    SBuffer[4]=('1');
    else    SBuffer[4]=('0');
    SBuffer[5]=('0');
    SBuffer[6]=('\r');
    Serial_Send_Raw(7,SBuffer,RS485_CHAR_DELAY);
}

void Serial_Send_PWM_Bright(uint8_t ID, uint8_t Ch1, uint8_t Ch2)
{
    SBuffer[0]=('P');
    SBuffer[1]=(ID);
    SBuffer[2]=('C');
    SBuffer[3]=('3');
    SBuffer[4]=(Ch1);
    SBuffer[5]=(Ch2);
    SBuffer[6]=('\r');
    Serial_Send_Raw(7,SBuffer,RS485_CHAR_DELAY);
}

void Serial_Send_PWM_ChBright(uint8_t ID, uint8_t Ch, uint8_t Val)
{
    
    SBuffer[0]=('P');
    SBuffer[1]=(ID);
    SBuffer[2]=('C');
    if (Ch==1)  SBuffer[3]=('1');
    if (Ch==2)  SBuffer[3]=('2');
    if (Ch==3)  SBuffer[3]=('4');
    SBuffer[4]=(Val);
    SBuffer[5]=('0');
    SBuffer[6]=('\r');
    Serial_Send_Raw(7,SBuffer,RS485_CHAR_DELAY);
}

void Serial_Send_PWM_MaxBright(uint8_t ID, uint8_t Val)
{
    
    SBuffer[0]=('P');
    SBuffer[1]=(ID);
    SBuffer[2]=('S');
    SBuffer[3]=('M');
    SBuffer[4]=(Val);
    SBuffer[5]=('0');
    SBuffer[6]=('\r');
    Serial_Send_Raw(7,SBuffer,RS485_CHAR_DELAY);
}

void Serial_Send_PWM_NightBright(uint8_t ID, uint8_t Val)
{
    
    SBuffer[0]=('P');
    SBuffer[1]=(ID);
    SBuffer[2]=('S');
    SBuffer[3]=('N');
    SBuffer[4]=(Val);
    SBuffer[5]=('0');
    SBuffer[6]=('\r');
    Serial_Send_Raw(7,SBuffer,RS485_CHAR_DELAY);
}

void Serial_Send_PWM_OnTime(uint8_t ID, uint8_t Val)
{
    
    SBuffer[0]=('P');
    SBuffer[1]=(ID);
    SBuffer[2]=('S');
    SBuffer[3]=('T');
    SBuffer[4]=(Val);
    SBuffer[5]=('0');
    SBuffer[6]=('\r');
    Serial_Send_Raw(7,SBuffer,RS485_CHAR_DELAY);
}

void Serial_Send_PWM_FadeTime(uint8_t ID, uint8_t Val)
{
    
    SBuffer[0]=('P');
    SBuffer[1]=(ID);
    SBuffer[2]=('S');
    SBuffer[3]=('R');
    SBuffer[4]=(Val);
    SBuffer[5]=('0');
    SBuffer[6]=('\r');
    Serial_Send_Raw(7,SBuffer,RS485_CHAR_DELAY);
}

void Serial_Send_Shutter_Start(uint8_t ID)
{
    SBuffer[0]=('S');
    SBuffer[1]=(ID);
    SBuffer[2]=('C');
    SBuffer[3]=('S');
    SBuffer[4]=('0');
    SBuffer[5]=('0');
    SBuffer[6]=('\r');
    Serial_Send_Raw(7,SBuffer,RS485_CHAR_DELAY);
}

void Serial_Send_Shutter_GoUp(uint8_t ID)
{
    SBuffer[0]=('S');
    SBuffer[1]=(ID);
    SBuffer[2]=('C');
    SBuffer[3]=('U');
    SBuffer[4]=('0');
    SBuffer[5]=('0');
    SBuffer[6]=('\r');
    Serial_Send_Raw(7,SBuffer,RS485_CHAR_DELAY);
}

void Serial_Send_Shutter_GoDown(uint8_t ID)
{
    SBuffer[0]=('S');
    SBuffer[1]=(ID);
    SBuffer[2]=('C');
    SBuffer[3]=('D');
    SBuffer[4]=('0');
    SBuffer[5]=('0');
    SBuffer[6]=('\r');
    Serial_Send_Raw(7,SBuffer,RS485_CHAR_DELAY);
}

void Serial_Send_Shutter_Position(uint8_t ID, uint8_t Position)
{
    SBuffer[0]=('S');
    SBuffer[1]=(ID);
    SBuffer[2]=('C');
    SBuffer[3]=('P');
    SBuffer[4]=(Position);
    SBuffer[5]=('0');
    SBuffer[6]=('\r');
    Serial_Send_Raw(7,SBuffer,RS485_CHAR_DELAY);
}

void Serial_Send_Raw(uint8_t Length, uint8_t *ToSend, uint8_t DelayTime)
{
    uint8_t i;
    for (i=0;i<Length;i++)
    {
        UART1_SendData8(ToSend[i]);
        while (UART1_SR_TXE == 0);
        if (DelayTime > 0) _delay_ms(DelayTime);
    }
}