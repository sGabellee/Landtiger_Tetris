#include "LPC17xx.h"
#include "button.h"
#include "timer.h"
#include "RIT.h"
#include "joystick.h"
#include "GLCD.h"
#include "tetris_game.h" 

int main (void) {
  SystemInit();
  BUTTON_init();
  joystick_init();
  LCD_Initialization();
  LCD_Clear(Black);

	ADC_init();
	Music_Init();
	
  // inizializzo il RIT in cui ci sono le logiche del joystick e dei tasti
  init_RIT(0x001312D0);
  enable_RIT();
	
	
	
	// inizializzo lo schermo inserendo la base (funzione game_init) e disegnando le linee e i testi 
  gameInit();
  LCD_DrawLine(FIELD_X - 1, FIELD_Y - 1, FIELD_X - 1, FIELD_Y + (BOARD_HEIGHT * BLOCK_SIZE) + 1, White);
  LCD_DrawLine(FIELD_X + (BOARD_WIDTH * BLOCK_SIZE) + 1, FIELD_Y - 1, FIELD_X + (BOARD_WIDTH * BLOCK_SIZE) + 1, FIELD_Y + (BOARD_HEIGHT * BLOCK_SIZE) + 1, White);
  LCD_DrawLine(FIELD_X - 1, FIELD_Y + (BOARD_HEIGHT * BLOCK_SIZE) + 1, FIELD_X + (BOARD_WIDTH * BLOCK_SIZE) + 1, FIELD_Y + (BOARD_HEIGHT * BLOCK_SIZE) + 1, White);
  GUI_Text(165, 40,  (uint8_t *) "TOP", White, Black);
  GUI_Text(165, 100, (uint8_t *) "SCORE", White, Black);
  GUI_Text(165, 160, (uint8_t *) "LINES", White, Black);
	
	// creo il primo pezzo e lo inserisco
  spawnNewPiece();
  drawTetromino(curr_x, curr_y, currPieceType, currRot, 1);
  
  gameState = 1;
  
  // faccio partire il timer di 1 secondo
  init_timer(0, 0x17D7840);
  enable_timer(0);

  //LPC_SC->PCON |= 0x1;	
  //LPC_SC->PCON &= 0xFFFFFFFD;
  //SCB->SCR |= 0x2;		
  __ASM("wfi");
}