#include "LPC17xx.h"
#include "timer.h"

void enable_timer( uint8_t timer_num ) {
  if ( timer_num == 0 ) LPC_TIM0->TCR = 1;
}

void disable_timer( uint8_t timer_num ) {
  if ( timer_num == 0 ) LPC_TIM0->TCR = 0;
}

void reset_timer( uint8_t timer_num ) {
  uint32_t regVal;
  if ( timer_num == 0 ) {
    regVal = LPC_TIM0->TCR;
    regVal |= 0x02;
    LPC_TIM0->TCR = regVal;
  }
}

uint32_t init_timer ( uint8_t timer_num, uint32_t TimerInterval ) {
  if ( timer_num == 0 ) {
    LPC_SC->PCONP |= (1<<1);
		
		//riadatto in base a come va l emulatore
    LPC_TIM0->PR  = 0;           
    
    LPC_TIM0->MR0 = TimerInterval; 
    LPC_TIM0->MCR = 3;
    NVIC_EnableIRQ(TIMER0_IRQn);
    NVIC_SetPriority(TIMER0_IRQn, 2); 

    return (0);
  }
  return (0);
}