#include "delay.h"

/*
 * Func delayed N cycles, where N = 3 + ( ticks * 3 )
 * so, ticks = ( N - 3 ) / 3, minimum delay is 6 CLK
 * when tick = 1, because 0 equels 65535
 */

static void _delay_cycl( uint16_t ticks )
{
/* COSMIC */
  #define T_COUNT(x) (( F_CPU * x / 1000000UL )-3)/3)
	// ldw X, __ticks ; insert automaticaly
	_asm("nop\n $N:\n decw X\n jrne $L\n nop\n ", ticks);
}

void _delay_us( const uint16_t us )
{
	_delay_cycl( (uint16_t)( T_COUNT(us) );
}

void _delay_ms( uint16_t ms )
{
	while ( ms-- )
	{
		_delay_us( 1000 );
	}
}

void Timer_Init(void)
{
  TIM4_TimeBaseInit(TIM4_MS_PRESC, TIM4_MS_PERIOD);
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  TIM4_Cmd(ENABLE);
}

