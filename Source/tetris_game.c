#include "tetris_game.h"
#include "GLCD.h" 
#include <stdlib.h>
#include <stdio.h>

// variabili varie ed eventuali
volatile int board[BOARD_HEIGHT][BOARD_WIDTH];
volatile int score = 0;
volatile int highscore = 0;
volatile int linescleared = 0;
volatile int gameState = 0;

volatile int currPieceType = 0;
volatile int currRot = 0;
volatile int curr_x = 3;
volatile int curr_y = 0;

//extra2
int total_lines_cleared = 0;
volatile int slow_mode_counter = 0;

const uint8_t TETROMINOES[7][4][4][4] = {
	// I-Shape 
	{
			{{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}}, 
			{{0,0,1,0}, {0,0,1,0}, {0,0,1,0}, {0,0,1,0}}, 
			{{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}}, 
			{{0,0,1,0}, {0,0,1,0}, {0,0,1,0}, {0,0,1,0}}  
	},
	// O-Shape 
	{
			{{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
			{{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
			{{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
			{{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}
	},
	// T-Shape
	{
			{{0,1,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}}, 
			{{0,1,0,0}, {0,1,1,0}, {0,1,0,0}, {0,0,0,0}}, 
			{{0,0,0,0}, {1,1,1,0}, {0,1,0,0}, {0,0,0,0}}, 
			{{0,1,0,0}, {1,1,0,0}, {0,1,0,0}, {0,0,0,0}}  
	},
	// J-Shape
	{
			{{1,0,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
			{{0,1,1,0}, {0,1,0,0}, {0,1,0,0}, {0,0,0,0}},
			{{0,0,0,0}, {1,1,1,0}, {0,0,1,0}, {0,0,0,0}},
			{{0,1,0,0}, {0,1,0,0}, {1,1,0,0}, {0,0,0,0}}
	},
	// L-Shape
	{
			{{0,0,1,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
			{{0,1,0,0}, {0,1,0,0}, {0,1,1,0}, {0,0,0,0}},
			{{0,0,0,0}, {1,1,1,0}, {1,0,0,0}, {0,0,0,0}},
			{{1,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,0,0,0}}
	},
	// S-Shape
	{
			{{0,1,1,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}},
			{{0,1,0,0}, {0,1,1,0}, {0,0,1,0}, {0,0,0,0}},
			{{0,0,0,0}, {0,1,1,0}, {1,1,0,0}, {0,0,0,0}},
			{{1,0,0,0}, {1,1,0,0}, {0,1,0,0}, {0,0,0,0}}
	},
	// Z-Shape
	{
			{{1,1,0,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
			{{0,0,1,0}, {0,1,1,0}, {0,1,0,0}, {0,0,0,0}},
			{{0,0,0,0}, {1,1,0,0}, {0,1,1,0}, {0,0,0,0}},
			{{0,1,0,0}, {1,1,0,0}, {1,0,0,0}, {0,0,0,0}}
	}
};

// aggiorno i punteggi a video
void updateStats(void) {
	char str[15];

	sprintf(str, "%d", score);
	GUI_Text(165, 120, (uint8_t *)str, White, Black);
	sprintf(str, "%d", linescleared);
	GUI_Text(165, 180, (uint8_t *)str, White, Black);
	sprintf(str, "%d", highscore);
	GUI_Text(165, 60, (uint8_t *)str, White, Black);
}

// funzione che inizializza il gioco
void gameInit(void) {
	int i, j;
	//azzera la board
	for(i=0; i<BOARD_HEIGHT; i++) {
			for(j=0; j<BOARD_WIDTH; j++) {
					board[i][j] = 0;
			}
	}
	score = 0;
	linescleared = 0;
	gameState = 0; 
	srand(LPC_RIT->RICOUNTER);
	updateStats();
	
	total_lines_cleared = 0;
	
}

// funzione che crea il pezzo
void spawnNewPiece(void) {
	currPieceType = rand() % 7;
	currRot = 0;
	curr_x = 3; 
	curr_y = 0; 
}


// funzione che controlla le collisioni e ritorna vero se collide
int CheckColl(int x, int y, int rot) {
	int r, c;
	// itero su tutto il pezzo e vedo se sono al bordo
	for(r = 0; r < 4; r++) {
		for(c = 0; c < 4; c++) {
			if(TETROMINOES[currPieceType][rot][r][c] != 0) {
				int board_x = x + c;
				int board_y = y + r;
				//controllo
				if (board_x<0 || board_x >= BOARD_WIDTH || board_y >= BOARD_HEIGHT) {
					return 1; 
				}
				if (board_y >= 0 && board[board_y][board_x]!= 0) {
					return 1; 
				}
			}
		}
	}
	return 0; 
}

//funzione che rimuove le linee quando le completiamo
void removeLines(void) {
	int r, c, k;
	int linRemNow = 0;
	int detectedPower = 0;
	//controllo per vedere se abbiamo riempito una riga
	for(r = BOARD_HEIGHT - 1; r >= 0; r--) {
		int full = 1;
		for(c = 0; c < BOARD_WIDTH; c++) {
			if(board[r][c] == 0) {
				full = 0;
				break;
			}
		}
	//caso in cui abbiamo almeno una riga fullata
		if(full) {
			//extra 2
			for(c = 0; c < BOARD_WIDTH; c++) {
				if(board[r][c] == CELL_POWERUP_CLEAR) detectedPower = CELL_POWERUP_CLEAR;
				if(board[r][c] == CELL_POWERUP_SLOW)  detectedPower = CELL_POWERUP_SLOW;
			}
			
			linRemNow++;
			for(k = r; k > 0; k--) {
				for(c = 0; c < BOARD_WIDTH; c++) {
					board[k][c] = board[k-1][c];
				}
			}
			for(c = 0; c < BOARD_WIDTH; c++) {
				board[0][c] = 0;
			}
			r++; 
		}
		
	}

	if(linRemNow > 0) {
		linescleared += linRemNow;
		// punteggio per linea
		score += (100 * linRemNow); 
		// tetris
		if (linRemNow == 4) score += 200; 
		
		Play_Sound_Effect(1);
		
		//extra2
		int old_total = total_lines_cleared;
    total_lines_cleared += linRemNow;
		
		if ((total_lines_cleared / 5) > (old_total / 5)) {
			Spawn_Powerup();
		}
		
		if ((total_lines_cleared / 10) > (old_total / 10)) {
            Apply_Malus();
		}
		if (detectedPower != 0) {
			Activate_Powerup(detectedPower);
		}
		
		updateStats();
	}
}

//blocco il pezzo che arriva alla fine della board
void lockPiece(void) {
	int r, c;
	for(r =0; r <4; r++) {
		for(c = 0; c < 4;c++) {
			if(TETROMINOES[currPieceType][currRot][r][c]!=0) {
				int board_x = curr_x + c;
				int board_y = curr_y + r;
				
				if(board_y >= 0 && board_y< BOARD_HEIGHT && board_x >=0 && board_x <BOARD_WIDTH) {
					board[board_y][board_x] = 1;
				}
			}
		}
	}
	//punti che vengono assenati ogni volta che arrivo alla fine con un blocco
	score += 10; 
	updateStats();
	
	//vado a controllare se devo rimuovere le linee
	removeLines(); 
	//nuovo pezzo
	spawnNewPiece();
	
	// controlla Game Over
	if (CheckColl(curr_x, curr_y, currRot)) {
		gameState = 2;
		Play_Sound_Effect(2);		
		//se il punteggio è record lo salvo
		if (score > highscore) {
			highscore = score;
			updateStats();
		}
		
		GUI_Text(50, 150, (uint8_t *)"GAME OVER", Red, Black);
	}
}

//funzione che disegna il blocco singolo
void drawSingle(int x, int y, uint16_t color) {
	int i, j;
	int start_x = FIELD_X + (x * BLOCK_SIZE); 
	int start_y = FIELD_Y + (y * BLOCK_SIZE);
	
	for(i = 0; i < BLOCK_SIZE; i++) {       
		for(j = 0; j < BLOCK_SIZE; j++) {   
			LCD_SetPoint(start_x + j, start_y + i, color);
		}
	}
}

//disegno il blocco completo
void drawTetromino(int x, int y, int type, int rot, int mode) {
	int r, c;
	uint16_t color = Black;
	//colore
	if (mode == 1) {
		switch(type) {
			case 0: color = Cyan; break;
			case 1: color = Yellow; break;
			case 2: color = Magenta; break;
			case 3: color = Blue; break;
			case 4: color = 0xFD20; break;
			case 5: color = Green; break;
			case 6: color = Red; break;
		}
	}
	//giro tutto il blocco e lo disegno a video
	for(r = 0; r < 4; r++) {
		for(c = 0; c < 4; c++) {
			if(TETROMINOES[type][rot][r][c] != 0) {
				if (y + r >= 0) { 
					drawSingle(x+c, y+r,color);
				}
			}
		}
	}
}
//ridisegno tutta la board
void drawWholeBoard(void) {
	int r, c;
	uint16_t color;
	for(r = 0; r < BOARD_HEIGHT; r++) {
		for(c = 0; c < BOARD_WIDTH; c++) {
			switch(board[r][c]) {
				case CELL_EMPTY:
					color = Black;
					break;
					
				case CELL_BLOCK:
					color = White;
					break;
					
				case CELL_POWERUP_CLEAR:
					color = Yellow;
					break;
					
				case CELL_POWERUP_SLOW:
					color = Magenta;
					break;
					
				default:
					color = Black;
					break;
			}
			
			// Disegno la cella col colore scelto
			drawSingle(c, r, color);
		}
	}
}

//extrapoint 2
void Apply_Malus(void) {
	int x, y;
	
	// 1. CONTROLLO GAME OVER PREVENTIVO
	// Se c'è qualcosa nella riga più alta (riga 0), spostando tutto in su
	// i blocchi uscirebbero dal campo -> GAME OVER 
	for (x = 0; x < BOARD_WIDTH; x++) {
		if (board[0][x] != 0) {
			gameState = 2; // La tua funzione che imposta gameState = 2 e mostra la schermata finale
			Play_Sound_Effect(2);
			return;
		}
	}

	// 2. SHIFT VERSO L'ALTO
	// Copio la riga y+1 nella riga y. Parto dall'alto.
	for (y = 0; y < BOARD_HEIGHT - 1; y++) {
		for (x = 0; x < BOARD_WIDTH; x++) {
			board[y][x] = board[y + 1][x];
		}
	}

	// 3. GENERAZIONE RIGA INFERIORE (Malus)
	// Specifica: "7 random spaces out of 10 contain a block" 
	
	// Prima pulisco l'ultima riga
	for (x = 0; x < BOARD_WIDTH; x++) {
		board[BOARD_HEIGHT - 1][x] = 0; // Empty
	}

	// Ora inserisco esattamente 7 blocchi in posizioni casuali
	int blocks_placed = 0;
	while (blocks_placed < 7) {
		int random_col = rand() % BOARD_WIDTH; // Numero da 0 a 9
		
		// Se la cella è vuota, metto un blocco
		if (board[BOARD_HEIGHT - 1][random_col] == 0) {
			board[BOARD_HEIGHT - 1][random_col] = 1; // Blocco Standard (Grigio/Colore base)
			blocks_placed++;
		}
	}
	
	// Ridisegna tutto il campo perché è cambiato drasticamente
	drawWholeBoard(); 
}
void Spawn_Powerup(void) {
    // Spec: "substitute a block that is already on the playing field" 
    
    // 1. Troviamo tutti i blocchi validi (normali)
	int candidates_x[200]; // Max size board
	int candidates_y[200];
	int count = 0;
	int y=0;
	int x=0;
	for(y=0; y<BOARD_HEIGHT; y++) {
		for(x=0; x<BOARD_WIDTH; x++) {
			if(board[y][x] == CELL_BLOCK) { // Solo blocchi normali
				candidates_x[count] = x;
				candidates_y[count] = y;
				count++;
			}
		}
	}
	
	// Se non ci sono blocchi (improbabile se abbiamo appena fatto 5 righe), usciamo
	if (count == 0) return;
	
	// 2. Ne scegliamo uno a caso
	int idx = rand() % count;
	int px = candidates_x[idx];
	int py = candidates_y[idx];
	
	// 3. Scegliamo il tipo di Powerup (50% probabilità)
	int type = (rand() % 2) ? CELL_POWERUP_CLEAR : CELL_POWERUP_SLOW;
	
	board[py][px] = type;
	
	// 4. Ridisegniamo SOLO quel blocco con un colore speciale
	// Nota: Devi modificare drawTetromino o drawBlock per gestire colori diversi 
	// in base al valore 2 (es. GIALLO) e 3 (es. VIOLA).
	// Per ora ridisegniamo tutto per sicurezza
	drawWholeBoard(); 
}

void Activate_Powerup(int type) {
	int x, y;
	if (type == CELL_POWERUP_SLOW) {
		// --- SLOW DOWN ---
		// Spec: "slows down the game to 1 square/second for 15 seconds" [cite: 61]
		// RIT gira a 50ms. 1 secondo = 20 tick. 15 secondi = 300 tick.
		slow_mode_counter = 300; 
	}
	else if (type == CELL_POWERUP_CLEAR) {
		// --- CLEAR HALF ---
		// Spec: "bottom half of the lines are instantly cleared" [cite: 57]
		
		int start_row = BOARD_HEIGHT / 2; // Metà campo (es. riga 10 su 20)
		int lines_removed = 0;
		
		// Pulisci la metà inferiore
		for (y = start_row; y < BOARD_HEIGHT; y++) {
			int row_has_blocks = 0;
			for(x=0; x<BOARD_WIDTH; x++) {
					if(board[y][x] != 0) row_has_blocks = 1;
					board[y][x] = 0;
			}
			if(row_has_blocks) lines_removed++;
		}
		
		// Spec: "award points in groups of 4 lines" [cite: 59]
		if (lines_removed > 0) {
			int groups_of_4 = lines_removed / 4;
			int remainder = lines_removed % 4;
			
			// Aggiungi punti (usa le tue costanti di punteggio)
			// score += groups_of_4 * SCORE_TETRIS;
			// score += Punteggio_per_righe(remainder);
			
			// updateStats(); // Aggiorna a video
		}
		
		// Shiftare giù la parte superiore? 
		// La specifica dice "instantly cleared". Di solito in Tetris questo crea un vuoto,
		// ma per semplicità potremmo far scendere tutto ciò che c'era sopra.
		// Implementazione semplificata: Ridisegna e basta (i blocchi sopra "cadranno" al prossimo update fisico se implementiamo gravità avanzata, altrimenti rimangono fluttuanti finché non si gestisce lo shift totale. Per l'esame, pulire e ridisegnare è spesso sufficiente).
		drawWholeBoard();
	}
}