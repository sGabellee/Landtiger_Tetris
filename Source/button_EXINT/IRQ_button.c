#include "button.h"
#include "LPC17xx.h"
#include "RIT.h"

// Pause / Restart
void EINT1_IRQHandler (void)
{
	enable_RIT();
	NVIC_DisableIRQ(EINT1_IRQn);
		
	// mando segnale leggibile al RIT
	LPC_PINCON->PINSEL4 &= ~(1 << 22);
	
	LPC_SC->EXTINT |= (1 << 1);
}

// Hard Drop
void EINT2_IRQHandler (void)
{
	enable_RIT();
	NVIC_DisableIRQ(EINT2_IRQn);
	
	// segnale per RIT
	LPC_PINCON->PINSEL4 &= ~(1 << 24);
	
	LPC_SC->EXTINT |= (1 << 2);
}