#pragma once

#include "config.h"
#include <stdint.h>

/* --------- Register defines --------- */
// Pin 1 (COL4)
#define KEYPAD_COL4			prt_PMODS_JA1
#define KEYPAD_COL4_TRIS	tris_PMODS_JA1
#define KEYPAD_COL4_RP		rp_PMODS_JA1

// Pin 2 (COL3)
#define KEYPAD_COL3			prt_PMODS_JA2
#define KEYPAD_COL3_TRIS	tris_PMODS_JA2
#define KEYPAD_COL3_RP		rp_PMODS_JA2

// Pin 3 (COL2)
#define KEYPAD_COL2			prt_PMODS_JA3
#define KEYPAD_COL2_TRIS	tris_PMODS_JA3
#define KEYPAD_COL2_RP		rp_PMODS_JA3

// Pin 4 (COL1)
#define KEYPAD_COL1			prt_PMODS_JA4
#define KEYPAD_COL1_TRIS	tris_PMODS_JA4
#define KEYPAD_COL1_RP		rp_PMODS_JA4
#define KEYPAD_COL1_ANSEL	ansel_PMODS_JA4

// Pin 7 (ROW4)
#define KEYPAD_ROW4			prt_PMODS_JA7
#define KEYPAD_ROW4_TRIS	tris_PMODS_JA7
#define KEYPAD_ROW4_RP		rp_PMODS_JA7

// Pin 8 (ROW3)
#define KEYPAD_ROW3			prt_PMODS_JA8
#define KEYPAD_ROW3_TRIS	tris_PMODS_JA8
#define KEYPAD_ROW3_RP		rp_PMODS_JA8
#define KEYPAD_ROW3_ANSEL	ansel_PMODS_JA8

// Pin 9 (ROW2)
#define KEYPAD_ROW2			prt_PMODS_JA9
#define KEYPAD_ROW2_TRIS	tris_PMODS_JA9
#define KEYPAD_ROW2_RP		rp_PMODS_JA9
#define KEYPAD_ROW2_ANSEL	ansel_PMODS_JA9

// Pin 10 (ROW1)
#define KEYPAD_ROW1			prt_PMODS_JA10
#define KEYPAD_ROW1_TRIS	tris_PMODS_JA10
#define KEYPAD_ROW1_RP		rp_PMODS_JA10
#define KEYPAD_ROW1_ANSEL	ansel_PMODS_JA10

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
