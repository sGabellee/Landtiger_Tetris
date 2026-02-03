#ifndef __ADC_H
#define __ADC_H

// Funzioni
void ADC_init(void);
void ADC_start_conversion(void);

// Variabile esterna (così il RIT la vede)
extern volatile uint16_t AD_current;

#endif