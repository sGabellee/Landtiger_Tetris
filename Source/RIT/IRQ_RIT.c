#include "LPC17xx.h"
#include "RIT.h"
#include "button.h"
#include "timer.h"
#include "tetris_game.h"
#include "GLCD.h"
#include "adc.h"
extern volatile int sound_duration;
#include "music.h"

void RIT_IRQHandler (void)
{			
	//azzero per dire che ho iniziato l'interrupt
	LPC_RIT->RICTRL |= 0x1;
	//variabili bottoni
	static int downKey1 = 0;
	static int downKey2 = 0;
	
	// variabilidel joystick
	static int J_left = 0;
	static int J_right = 0;
	static int J_select = 0; 
	static int J_down_active = 0;

	
	//extrapoint2
	
	if (sound_duration > 0) {
		sound_duration--;
		if (sound_duration == 0) {
			Stop_Sound(); // Spegne quando il tempo scade
		}
	}
	
	if(LPC_ADC->ADDR5 & 0x80000000){
		AD_current = ((LPC_ADC->ADDR5 >> 4) & 0xFFF);
	}
	LPC_ADC->ADCR |= (1 << 24); 
	
	int speed_factor = 1 + (AD_current * 4) / 0xFFF;
	uint32_t target_MR0 = 25000000 / speed_factor;
	
	// Gestione Powerup Slow
	if (slow_mode_counter > 0) {
		target_MR0 = 25000000; 
		slow_mode_counter--;   
	}
	
	// Joystick DOWN (Raddoppia velocità)
	if(gameState == 1 && (LPC_GPIO1->FIOPIN & (1<<26)) == 0){ 
		target_MR0 = target_MR0 / 2;
	}

	// 4. Smart Update del Timer
	if(gameState == 1 && LPC_TIM0->MR0 != target_MR0) {
		LPC_TIM0->MR0 = target_MR0; 
		if(LPC_TIM0->TC > target_MR0) {
			LPC_TIM0->TC = 0;
		}
	}
	if (target_MR0 < 1000000) {
        target_MR0 = 1000000; // Bloccalo a un valore sicuro (25 frame/sec)
  }
	
	
	// gestione hoystick
	if(gameState==1){
		
		// sinistra
		if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){ 
			J_left++;
			//movimento rapido: primo tocco (1) o ripetizione ogni 2 tick dopo un'attesa
			if(J_left == 1 || (J_left>2 && J_left%100 == 0)){
				if(!CheckColl(curr_x - 1, curr_y, currRot)){
					drawTetromino(curr_x, curr_y, currPieceType, currRot, 0);
					curr_x--;
					drawTetromino(curr_x, curr_y, currPieceType, currRot, 1);
				}
			}
		} else { J_left = 0; }

		//	destra (logica identica a sinistra)
		if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){ 
			J_right++;
			if(J_right == 1 || (J_right > 2 && J_right % 100 == 0)){
				if(!CheckColl(curr_x + 1, curr_y, currRot)){
					drawTetromino(curr_x, curr_y, currPieceType, currRot, 0);
					curr_x++;
					drawTetromino(curr_x, curr_y, currPieceType, currRot, 1);
				}
			}
		} else { J_right = 0; }
		
		// rotazione (joystick in alto)
		if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){ 
			J_select++;
			//non voglio far roteare in continuazione quindi lo faccio fare solo per un tick poi riazzero dentro l else
			switch(J_select){
				case 1: 
				{
					//controllo se vado a sbattere e poi ruoto
					int newRot = (currRot + 1) % 4;
					if(!CheckColl(curr_x, curr_y, newRot)){
						drawTetromino(curr_x, curr_y, currPieceType, currRot, 0);
						currRot = newRot;
						drawTetromino(curr_x, curr_y, currPieceType, currRot, 1);
						//extra2
						Play_Sound_Effect(0);
					}
				}
					break;
				default: break;
			}
		}
		else{J_select = 0; }
		
		// accelero blocco
		/*if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){ 
			// Specifica: Raddoppia velocità corrente (dimezza tempo) [cite: 51]
			uint32_t fast_MR0 = current_MR0 / 2;
			
			if(LPC_TIM0->MR0 != fast_MR0){
				init_timer(0, fast_MR0); 
			}
			J_down_active = 1;
		} 
		else { // Rilascio
			if(J_down_active == 1 || LPC_TIM0->MR0 != current_MR0){
                // Se rilascio O se ho girato il potenziometro mentre non premevo
				if(LPC_TIM0->MR0 != current_MR0){
						init_timer(0, current_MR0); 
				}
				J_down_active = 0;
			}	
		}*/
	}
	//KEY1
	if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){
		downKey1++;
		switch(downKey1){
			case 1:
				// PAUSA -> PLAY
				if (gameState == 0) { 
					gameState = 1; 
					GUI_Text(100, 150, (uint8_t *)"      ", Black, Black); 
					enable_timer(0); 
				} 
				// PLAY -> PAUSA
				else if (gameState == 1) { 
					gameState = 0; 
					disable_timer(0); 
					GUI_Text(100, 150, (uint8_t *)"PAUSED", Red, Black);
				} 
				// GAMEOVER -> RESTART
				else if (gameState == 2) { 
					//faccio ripartire tutto
					gameInit();
					LCD_Clear(Black);
					// ridisegna bordi
					LCD_DrawLine(FIELD_X - 1, FIELD_Y - 1, FIELD_X - 1, FIELD_Y + (BOARD_HEIGHT * BLOCK_SIZE) + 1, White);
					LCD_DrawLine(FIELD_X + (BOARD_WIDTH * BLOCK_SIZE) + 1, FIELD_Y - 1, FIELD_X + (BOARD_WIDTH * BLOCK_SIZE) + 1, FIELD_Y + (BOARD_HEIGHT * BLOCK_SIZE) + 1, White);
					LCD_DrawLine(FIELD_X - 1, FIELD_Y + (BOARD_HEIGHT * BLOCK_SIZE) + 1, FIELD_X + (BOARD_WIDTH * BLOCK_SIZE) + 1, FIELD_Y + (BOARD_HEIGHT * BLOCK_SIZE) + 1, White);
					GUI_Text(165, 40,  (uint8_t *) "TOP", White, Black);
					GUI_Text(165, 100, (uint8_t *) "SCORE", White, Black);
					GUI_Text(165, 160, (uint8_t *) "LINES", White, Black);
					updateStats();
					spawnNewPiece();
					// primo pezzo
					drawTetromino(curr_x, curr_y, currPieceType, currRot, 1);
					//faccio ripartire subito 
					gameState = 1; 
					init_timer(0, 0x17D7840); 
					enable_timer(0);
				}
				break;
			default: break;
		}
	} else {	
		if (downKey1 > 0) {
			//faccio ripartire il conteggio cosi quando passa a 1 entra nel codice
			downKey1 = 0;			
			NVIC_EnableIRQ(EINT1_IRQn);
			LPC_PINCON->PINSEL4 |= (1 << 22);
		}
	}

	// KEY2
	if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){
		downKey2++;
		switch(downKey2){
			case 1: 
				if (gameState == 1) {
					//scendi finché non tocchi e disegna
					while(CheckColl(curr_x, curr_y +1,currRot) ==0) {
						drawTetromino(curr_x, curr_y, currPieceType, currRot, 0);
						curr_y++;
					}
					drawTetromino(curr_x, curr_y, currPieceType, currRot, 1);
					
					// controllo se ho cancellato righe e in caso rifaccio la board
					int oldLines = linescleared;
					lockPiece();
					if(linescleared > oldLines) {
						drawWholeBoard();
					}
					if(gameState == 1) {
						drawTetromino(curr_x, curr_y, currPieceType, currRot, 1);
					}
				}
				break;
			default: break;
		}
	} else {	
		//uguale a key1
		if (downKey2 > 0) {
			downKey2 = 0;
			NVIC_EnableIRQ(EINT2_IRQn);
			LPC_PINCON->PINSEL4 |= (1 << 24);
		}
	}

}