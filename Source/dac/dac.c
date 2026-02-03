#include "LPC17xx.h"
#include "dac.h"

void DAC_init(void) {
	// Configura P0.26 come AOUT (Uscita Analogica per lo Speaker)
	// PINSEL1: Bit 21:20 a 10 (binario) -> Funzione DAC
	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	
	// Configura la modalità elettrica (niente resistenze)
	LPC_PINCON->PINMODE1 |= (1<<21); 
	LPC_PINCON->PINMODE1 &= ~(1<<20);
}