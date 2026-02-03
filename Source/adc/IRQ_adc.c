#include "LPC17xx.h"
#include "adc.h"


volatile uint16_t AD_current = 0;   

void ADC_IRQHandler(void) {
}