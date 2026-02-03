#include "LPC17xx.h"
#include "timer.h"
#include "../tetris_game.h"
	
void TIMER0_IRQHandler (void)
{
	LPC_TIM0->IR = 1;
	//caso game fermo
	if (gameState != 1) return;

	//logica caduta ossia lo cancello provo a farlo scendere e controllo se può farlo
	drawTetromino(curr_x, curr_y, currPieceType, currRot, 0);
	curr_y++;
	if (CheckColl(curr_x, curr_y, currRot)) {
		//caso in cui collido
		curr_y--;
		
		// ridisegno il pezzo dov era
		drawTetromino(curr_x, curr_y, currPieceType, currRot, 1);

		int oldLines = linescleared;
		// lo blocco li
		lockPiece(); 
		// nel caso in cui ho cancellato linee ridisegno tutto
		if (linescleared > oldLines) {
			drawWholeBoard();
		}
		// disegno il pezzo creato in lock
		if (gameState == 1) {
			 drawTetromino(curr_x, curr_y, currPieceType, currRot, 1);
		}
	} else {
		// non collido allora disegno
		drawTetromino(curr_x, curr_y, currPieceType, currRot, 1);
	}

	return;
}

static int wave_state = 0;

void TIMER1_IRQHandler (void) {
	LPC_TIM1->IR = 1; // Pulisci flag interrupt
	
	// Genera Onda Quadra sul DAC
	if (wave_state == 0) {
		LPC_DAC->DACR = (0x3FF << 6); // Volume Alto (Metà scala)
		wave_state = 1;
	} else {
		LPC_DAC->DACR = 0;            // Volume Basso
		wave_state = 0;
	}
}
void TIMER2_IRQHandler (void) { LPC_TIM2->IR = 1; }
void TIMER3_IRQHandler (void) { LPC_TIM3->IR = 1; }