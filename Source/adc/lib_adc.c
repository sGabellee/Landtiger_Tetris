#include "LPC17xx.h"
#include "adc.h"

void ADC_init (void) {
  // 1. Accendi ADC
  LPC_SC->PCONP |= (1 << 12);

  // 2. Configura P1.31
  LPC_PINCON->PINSEL3 |= (3UL << 30); 

  // 3. Configura ADCR:
  // - Bit 5: Select AD0.5
  // - Bit 8-15: Clock Div (4) -> 25MHz/5 = 5MHz
  // - Bit 21: PDN (Acceso)
  // - Bit 24: START (0 = No start, lo facciamo via software)
  // - Bit 16: BURST (0 = Spento, controllo manuale)
  LPC_ADC->ADCR = (1<<5) | (4<<8) | (1<<21); 

  // 4. DISABILITA INTERRUPT (Importante per il polling)
  LPC_ADC->ADINTEN = 0; 
  NVIC_DisableIRQ(ADC_IRQn);
}

void ADC_start_conversion (void) {
  // Avvia conversione ORA (Bit 24)
  LPC_ADC->ADCR |= (1 << 24);
}