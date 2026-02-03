#ifndef MUSIC_H
#define MUSIC_H

#include <stdint.h>

// Frequenze Note (in Hz)
#define NOTE_C4  262
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_C5  523
#define PAUSE    0

// Durate (in tick del RIT da 50ms)
#define DURATION_BEEP   2   // 100ms
#define DURATION_SHORT  5   // 250ms
#define DURATION_LONG   30  // 1.5s

// Funzioni
void Music_Init(void);
void Play_Note(int frequency, int duration_ticks);
void Stop_Sound(void);

// AGGIUNGI QUESTA RIGA:
void Play_Sound_Effect(int type);

#endif