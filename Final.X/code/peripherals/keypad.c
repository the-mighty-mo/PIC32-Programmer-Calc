/*
 * Module for the PmodKYPD peripheral.
 *
 * Author: Benjamin Hall
 * Date: 2022 September 16
 */

#include "peripherals/keypad.h"

void Keypad_Init(void)
{
	KEYPAD_COL1_ANSEL = 0; // disable analog input
	KEYPAD_COL1_RP = 0; // disable remappable pin
	KEYPAD_COL1_TRIS = 0; // set pin as output

    KEYPAD_COL2_RP = 0; // disable remappable pin
    KEYPAD_COL2_TRIS = 0; // set pin as output

	KEYPAD_COL3_RP = 0; // disable remappable pin
	KEYPAD_COL3_TRIS = 0; // set pin as output

	KEYPAD_COL4_RP = 0; // disable remappable pin
	KEYPAD_COL4_TRIS = 0; // set pin as output

	KEYPAD_ROW1_ANSEL = 0; // disable analog input
	KEYPAD_ROW1_RP = 0; // disable remappable pin
	KEYPAD_ROW1_TRIS = 1; // set pin as input

	KEYPAD_ROW2_ANSEL = 0; // disable analog input
	KEYPAD_ROW2_RP = 0; // disable remappable pin
	KEYPAD_ROW2_TRIS = 1; // set pin as input

	KEYPAD_ROW3_ANSEL = 0; // disable analog input
	KEYPAD_ROW3_RP = 0; // disable remappable pin
	KEYPAD_ROW3_TRIS = 1; // set pin as input

	KEYPAD_ROW4_RP = 0; // disable remappable pin
	KEYPAD_ROW4_TRIS = 1; // set pin as input
}

static uint8_t const keys[4][4] = {
	{0x1, 0x2, 0x3, 0xA},
	{0x4, 0x5, 0x6, 0xB},
	{0x7, 0x8, 0x9, 0xC},
	{0x0, 0xF, 0xE, 0xD}
};

/**
 * Gets the first row with buttons pushed only on any of the given columns.
 * 
 * @param cols
 *        The columns to test; each bit represents a column, 1 to test
 */
static uint8_t GetRow(uint8_t cols)
{
	uint8_t retval = 0;

	/* set the columns specified; logic low activates the column */
	KEYPAD_COL1 = ~(cols >> 0) & 0x1;
	KEYPAD_COL2 = ~(cols >> 1) & 0x1;
	KEYPAD_COL3 = ~(cols >> 2) & 0x1;
	KEYPAD_COL4 = ~(cols >> 3) & 0x1;

	/* find the first row with a pressed button; logic low when pressed */
	if (!KEYPAD_ROW1) {
		retval = 1;
	} else if (!KEYPAD_ROW2) {
		retval = 2;
	} else if (!KEYPAD_ROW3) {
		retval = 3;
	} else if (!KEYPAD_ROW4) {
		retval = 4;
	} else {
		retval = 0;
	}

	/* disable all columns */
	KEYPAD_COL1 = 1;
	KEYPAD_COL2 = 1;
	KEYPAD_COL3 = 1;
	KEYPAD_COL4 = 1;

	return retval;
}

int8_t Keypad_GetKey(void)
{
	/* get the first active row with all columns active */
	uint8_t const row = GetRow(0xF);
	if (!row) {
		/* no buttons pressed */
		return -1;
	}

	/* find the columns with buttons pressed on the discovered row ("active" columns) */
	uint8_t active_cols = 0xF;
	for (int i = 0; i < 4; ++i) {
		/* check all columns except for i */
		uint8_t const cols = active_cols & ~(1 << i);
		/* get the first active row with the set columns */
		uint8_t const cols_row = GetRow(cols);
		if (cols_row == row) {
			/* button i is not pressed, remove it from active_cols */
			active_cols = cols;
		}
	}

	/* check if there are any active columns for the row */
	if (active_cols) {
		/* find the leftmost active column (least significant bit) */
		uint8_t col = 1;
		while (!(active_cols & 1)) {
			active_cols >>= 1;
			++col;
		}
		/* return the key for the row and column; row and column starts at 1 */
		return keys[row - 1][col - 1];
	} else {
		/* this shouldn't be possible; no active columns */
		return -1;
	}
}
