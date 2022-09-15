#include "peripherals/keypad.h"

static int8_t key = -1;
static int8_t last_key = -1;

void Keypad_Init()
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

	KEYPAD_COL1 = 1;
	KEYPAD_COL2 = 1;
	KEYPAD_COL3 = 1;
	KEYPAD_COL4 = 1;

	return retval;
}

void Keypad_Process()
{
	uint8_t const row = GetRow(0xF);
	if (!row) {
		key = -1;
		return;
	}

	uint8_t active_cols = 0xF;
	for (int i = 0; i < 4; ++i) {
		uint8_t const cols = active_cols & ~(1 << i);
		uint8_t const cols_row = GetRow(cols);
		if (cols_row == row) {
			active_cols = cols;
		}
	}

	if (active_cols) {
		uint8_t col = 1;
		while (!(active_cols & 1)) {
			active_cols >>= 1;
			++col;
		}
		key = keys[row - 1][col - 1];
	} else {
		key = -1;
	}
}

uint8_t Keypad_IsNewKey()
{
	uint8_t const new_key = key != last_key;
	last_key = key;
	return new_key;
}

int8_t Keypad_GetKey()
{
	return key;
}
