# 🧱 Tetris - LandTiger LPC1768 Edition

A feature-rich implementation of the classic Tetris game for the NXP LPC1768 LandTiger development board. This project includes advanced mechanics such as dynamic speed control via ADC, a unique Power-up/Malus system, TFT LCD graphics, and polyphonic background music.

---

## 🎮 Controls & Hardware

| Input | Action | Description |
| :--- | :--- | :--- |
| **Joystick LEFT** | Move Left | Moves the tetromino one step to the left. |
| **Joystick RIGHT** | Move Right | Moves the tetromino one step to the right. |
| **Joystick UP** | Rotate | Rotates the current piece 90 degrees clockwise. |
| **Joystick DOWN** | Soft Drop | Doubles the falling speed while held. |
| **KEY 1 (INT0)** | Start / Pause | **First Press:** Seeds the RNG using the RIT counter and starts the game.<br>**Subsequent:** Toggles Pause/Play. |
| **KEY 2 (INT0)** | Hard Drop | Instantly drops the piece to the bottom and locks it. |
| **Potentiometer** | Speed Control | Real-time adjustment of the game speed (falling rate). |

---

## 🚀 Core Mechanics

### 1. True Random Generation
Unlike standard pseudo-random implementations, this game guarantees a unique sequence of blocks every time.
* The game starts in a "Paused" state.
* The exact microsecond (`LPC_RIT->RICOUNTER`) the user presses **KEY 1** is used to seed the random number generator (`srand`).
* The initial default piece is immediately swapped for a random one upon start.

### 2. Dynamic Analog Speed
The falling speed of the blocks is controlled by the on-board **ADC (Analog-to-Digital Converter)** connected to the potentiometer.
* **Rotate Clockwise:** Increases game speed (High Difficulty).
* **Rotate Counter-Clockwise:** Decreases game speed (Low Difficulty).
* *Safety features prevent the timer from going too fast and crashing the board.*

---

## ⚡ Special Features (Power-ups & Malus)

The game features a dynamic event system triggered by the total number of lines cleared.

### 🎁 Power-ups (Trigger: Every 5 Lines)
Every 5 lines cleared, a random standard block on the field transforms into a Power-up block. Clearing the line containing this block activates the effect.

* **🟨 CLEAR HALF (Yellow Block)**
    * **Effect:** Instantly clears the bottom half of the game board (rows 10-19).
    * **Score Bonus:** Awards points for all cleared lines plus a "Tetris" bonus multiplier.
  
* **🟪 SLOW DOWN (Magenta Block)**
    * **Effect:** Overrides the potentiometer and forces the game to **Slow Mode** (1 block/sec) for **15 seconds**.
    * **Use Case:** Perfect for recovering when the game speed becomes unmanageable.

### ⚠️ Malus (Trigger: Every 10 Lines)
Every 10 lines cleared, the game increases difficulty via a penalty system.

* **♻️ GARBAGE LINE**
    * **Effect:** All existing blocks are shifted up by one row.
    * **Penalty:** A new line appears at the very bottom containing **7 random solid blocks**.
    * **Risk:** If blocks are touching the top of the screen, this shift triggers an immediate **GAME OVER**.

---

## 🎵 Audio System

The project uses the DAC and Timer1 to generate audio.

* **Soundtrack:** A custom 8-bit rendition of *"Never Gonna Give You Up"* (Rick Astley).
    * The music plays in a continuous loop during gameplay.
    * Rhythm and tempo are synchronized with the RIT.
* **SFX Priority System:** Sound effects (Rotation beep, Line clear cheer, Game Over) take priority over the music, pausing the soundtrack momentarily to ensure clear audio feedback.

---

## 🛠 Technical Implementation Details

* **Microcontroller:** NXP LPC1768 (ARM Cortex-M3).
* **Display:** GLCD (Graphical LCD) library for 320x240 TFT.
* **RIT (Repetitive Interrupt Timer):** Handles the main game loop, polling (Joystick/ADC), and music timing (50ms tick).
* **Timer 0:** Dedicated to the gravity of the falling pieces.
* **Timer 1:** Dedicated to generating square waves for the DAC (Audio).
* **Optimization:** Heavy arrays used for Power-up logic are declared `static` to prevent Stack Overflow during interrupts.

## 🏁 How to Play

1. **Flash** the board with the compiled binary.
2. **Reset** the board. You will see the initial screen and "PRESS KEY1".
3. **Press KEY 1** to start.
4. Use the **Joystick** to stack blocks.
5. Use the **Potentiometer** to adjust difficulty.
6. Aim for the colored blocks to trigger Power-ups!
