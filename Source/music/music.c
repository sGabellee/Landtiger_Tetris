#include "LPC17xx.h"
#include "music.h"
#include "dac.h"

volatile int sound_duration = 0; 

void Music_Init(void) {
	DAC_init(); 
	LPC_SC->PCONP |= (1 << 2); 
	LPC_TIM1->MCR = 3;         
	NVIC_EnableIRQ(TIMER1_IRQn);
}

void Play_Note(int frequency, int duration_ticks) {
	if (frequency == 0) {
		Stop_Sound();
		return;
	}
	uint32_t ticks = (25000000 / frequency) / 2;
	LPC_TIM1->TCR = 2; // Reset
	LPC_TIM1->TCR = 0; 
	LPC_TIM1->MR0 = ticks;
	sound_duration = duration_ticks;
	LPC_TIM1->TCR = 1; // Start
}

void Stop_Sound(void) {
	LPC_TIM1->TCR = 0; 
	LPC_DAC->DACR = 0; 
}

// --- AGGIUNGI QUESTA FUNZIONE ALLA FINE ---
void Play_Sound_Effect(int type) {
	switch(type) {
		case 0: // ROTAZIONE (Suono acuto breve)
			Play_Note(NOTE_C5, DURATION_BEEP); 
			break;
			
		case 1: // LINE CLEAR (Suono medio di vittoria)
			Play_Note(NOTE_G4, DURATION_SHORT); 
			break;
			
		case 2: // GAME OVER (Suono grave lungo)
			Play_Note(NOTE_C4, DURATION_LONG); 
			break;
	}
}