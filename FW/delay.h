/*
 * delay utilite for STM8 family
 * COSMIC and SDCC
 * Terentiev Oleg
 * t.oleg@ymail.com
 */

#include "stm8s.h"

#ifndef _UTIL_DELAY_H_
#define _UTIL_DELAY_H_ 1

#define F_CPU 16000000UL

#define TIM4_MS_PERIOD										124
#define TIM4_MS_PRESC											(TIM4_PRESCALER_128)

//#define TIM4_US_PERIOD										4
//#define TIM4_US_PRESC											(TIM4_PRESCALER_4)

#define TIM4_WS2811_800K_PERIOD_T0H				2			//(200/350/500ns) 2x200=400ns
#define TIM4_WS2811_800K_PERIOD_T0L 			4			//(550/700/850ns) 4x200=800ns
#define TIM4_WS2811_800K_PERIOD_TL				4			//(450/600/950ns) 4x200=800ns
#define TIM4_WS2811_800K_PRESC						(TIM4_PRESCALER_2)

#define WS2811_800K_US_RESET							60


static void _delay_cycl( uint16_t ticks );


void _delay_us( const uint16_t us );


void _delay_ms( uint16_t ms );

void Timer_Init(void);
void Timer_Handle(void);


#endif
