#pragma once

#include <xc.h>
#include <stdint.h>

/* --------- Register defines --------- */
// Pin 1 (COL4)
#define KEYPAD_COL4			PORTCbits.RC2
#define KEYPAD_COL4_TRIS	TRISCbits.TRISC2
#define KEYPAD_COL4_RP		RPC2R

// Pin 2 (COL3)
#define KEYPAD_COL3			PORTCbits.RC1
#define KEYPAD_COL3_TRIS	TRISCbits.TRISC1
#define KEYPAD_COL3_RP		RPC1R

// Pin 3 (COL2)
#define KEYPAD_COL2			PORTCbits.RC4
#define KEYPAD_COL2_TRIS	TRISCbits.TRISC4
#define KEYPAD_COL2_RP		RPC4R

// Pin 4 (COL1)
#define KEYPAD_COL1			PORTGbits.RG6
#define KEYPAD_COL1_TRIS	TRISGbits.TRISG6
#define KEYPAD_COL1_RP		RPG6R
#define KEYPAD_COL1_ANSEL	ANSELGbits.ANSG6

// Pin 7 (ROW4)
#define KEYPAD_ROW4			PORTCbits.RC3
#define KEYPAD_ROW4_TRIS	TRISCbits.TRISC3
#define KEYPAD_ROW4_RP		RPC3R

// Pin 8 (ROW3)
#define KEYPAD_ROW3			PORTGbits.RG7
#define KEYPAD_ROW3_TRIS	TRISGbits.TRISG7
#define KEYPAD_ROW3_RP		RPG7R
#define KEYPAD_ROW3_ANSEL	ANSELGbits.ANSG7

// Pin 9 (ROW2)
#define KEYPAD_ROW2			PORTGbits.RG8
#define KEYPAD_ROW2_TRIS	TRISGbits.TRISG8
#define KEYPAD_ROW2_RP		RPG8R
#define KEYPAD_ROW2_ANSEL	ANSELGbits.ANSG8

// Pin 10 (ROW1)
#define KEYPAD_ROW1			PORTGbits.RG9
#define KEYPAD_ROW1_TRIS	TRISGbits.TRISG9
#define KEYPAD_ROW1_RP		RPG9R
#define KEYPAD_ROW1_ANSEL	ANSELGbits.ANSG9

/* --------- Functions --------- */

/**
 * Initializes the keypad module.
 */
void Keypad_Init();
/**
 * Returns the first button found to be pressed.
 * Priority is given to row 1, column 1, and row is prioritized above column.
 */
int8_t Keypad_GetKey();
