#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#include "LPC17xx.h"

#define BOARD_HEIGHT 20
#define BOARD_WIDTH  10
#define BLOCK_SIZE   15   
#define FIELD_X      5    
#define FIELD_Y      10

//extra2
#define CELL_EMPTY          0  // Cella vuota
#define CELL_BLOCK          1  // Blocco normale (quello standard)
#define CELL_POWERUP_CLEAR  2  // Powerup "O" (Clear Half Lines)
#define CELL_POWERUP_SLOW   3

// 7 pezzi, 4 rotazioni possibili, griglia 4x4 per ogni pezzo 0=Vuoto, 1=Parte del pezzo
extern const uint8_t TETROMINOES[7][4][4][4];

// board
extern volatile int board[BOARD_HEIGHT][BOARD_WIDTH];

// variabili di giooc
extern volatile int score;
extern volatile int highscore;
extern volatile int linescleared;
extern volatile int gameState; 
//variabili del blocco
extern volatile int currPieceType;
extern volatile int currRot;
extern volatile int curr_x;
extern volatile int curr_y;

//extra2
extern int total_lines_cleared;
void Apply_Malus(void);
extern volatile int slow_mode_counter;
void Spawn_Powerup(void);
void Activate_Powerup(int type);

void gameInit(void);
void spawnNewPiece(void);
int CheckColl(int x, int y, int rot);
void lockPiece(void);
void removeLines(void);
void drawTetromino(int x, int y, int type, int rot, int mode);
void drawWholeBoard(void);
#endif