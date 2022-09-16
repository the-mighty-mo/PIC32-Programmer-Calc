#include "calculator.h"
#include "peripherals/btn.h"
#include "peripherals/lcd.h"
#include "peripherals/led.h"
#include "peripherals/rgbled.h"
#include "input.h"
#include "utils.h"
#include <string.h>

// Operands
static uint16_t nums[2];
static uint8_t num_idx;

// Numerical base
static enum NumBase {
	Bin,
	Dec,
	Hex
} num_base;
static uint8_t const bases[] = {2, 10, 16};

// Operator
static enum Operator {
	Add,
	Sub,
	Mult,
	Div,
	And,
	Or,
	Xor
} operator;

// LCD output
static char lcd[2][17] = {0};
static uint8_t update_lcd[2] = {0};
static char const operators[] = {'+', '-', '*', '/', '&', '|', '^'};

// RGBLED output
static union {
	// whether any operation wants the RGB LED red
	uint8_t is_red;
	// a bitfield of operations that can set the RGB LED
	struct RgbRedStatus {
		// first operand
		uint8_t num1 : 1;
		// second operand
		uint8_t num2 : 1;
		// last result
		uint8_t result : 1;
	} fields;
} rgb_is_red = {0};
static uint8_t last_rgb_is_red = 0;

// Private functions
static void RunOp(void);
static void UpdateNum(uint8_t key);
static void UpdateNumLcd(void);
static void UpdateNumRgbLed(void);
static void NumToStr(uint16_t num, char *str, size_t strlen);

/** Resets the operands and operator. */
static void ResetNums(void)
{
	memset(nums, 0, sizeof(nums));
	num_idx = 0;
	memset(&rgb_is_red, 0, sizeof(rgb_is_red));
}

/** Clears the LCD output. */
static void ClearLcd(void)
{
	// First line: first operand
	NumToStr(nums[0], lcd[0] + 1, sizeof(lcd[0]) - 2);
	lcd[0][0] = ' ';

	// Second line: operator and spaces
	memset(lcd[1], ' ', sizeof(lcd[1]) - 1);
	lcd[1][0] = operators[operator];

	// Signal that we need to write to the LCD
	update_lcd[0] = 1;
	update_lcd[1] = 1;
}

/** Clears the RGB LED. */
static void ClearRgb(void)
{
	memset(&rgb_is_red, 0, sizeof(rgb_is_red));
	// assume RGB was red so we turn it off
	last_rgb_is_red = 1;
}

void Calculator_Init(void)
{
	// Reset the operands and operator
	ResetNums();
	num_base = Hex;
	operator = Add;
	// Clear the LCD display
	ClearLcd();
	// Clear the RGB LED
	ClearRgb();
}

/**
 * Reads the switches and updates the current operator.
 */
static void ProcessOperator(void)
{
	uint8_t const swt = Input_GetSwtGroup();
	// Note: swt & (swt - 1) results in swt with the rightmost 1 flipped to 0
	if (Input_IsNewSwtGroup() && (swt & (swt - 1)) == 0) {
		// only one switch is set, set operation
		for (int i = 0; i < sizeof(operators) / sizeof(*operators); ++i) {
			if (swt & (1 << i)) {
				// found the operator
				operator = i;
				// update the operator on the LCD
				lcd[1][0] = operators[operator];
				update_lcd[1] = 1;
				break;
			}
		}
	}
	// Set the LED of the active operator
	LED_SetGroupValue(1 << operator);
}

/** Updates the numerical base on the LCD display. */
static void UpdateBase(void)
{
	// Update the LCD string to reflect the new base
	NumToStr(nums[0], lcd[0] + 1, sizeof(lcd[0]) - 2);
	update_lcd[0] = 1;

	// Also update the second operand if we're on it
	if (num_idx) {
		NumToStr(nums[1], lcd[1] + 1, sizeof(lcd[1]) - 2);
		update_lcd[1] = 1;
	}

	// Update the RGB LED for the current operand
	UpdateNumRgbLed();
}

/**
 * Reads the buttons and updates the numerical base.
 */
static void ProcessNumBase(void)
{
	if (Input_GetNewBtn(BTN_U_BIT)) {
		if (++num_base > Hex) {
			num_base = Bin;
		}
		UpdateBase();
	} else if (Input_GetNewBtn(BTN_D_BIT)) {
		if (num_base-- == 0) {
			num_base = Hex;
		}
		UpdateBase();
	}
}

/**
 * Reads the buttons and clears or deletes characters from the input.
 */
static void ProcessClearBackspace(void)
{
	if (Input_GetNewBtn(BTN_R_BIT)) {
		if (nums[num_idx] != 0) {
			nums[num_idx] = 0;
			UpdateNumLcd();
		} else if (num_idx > 0) {
			ResetNums();
			ClearLcd();
		}

		// update the RGB LED for the current operand
		UpdateNumRgbLed();
		// disable red LED for last result, user wants to use what's left
		rgb_is_red.fields.result = 0;
	} else if (Input_GetNewBtn(BTN_L_BIT) && nums[num_idx]) {
		// shift out the most recent digit (least significant))
		nums[num_idx] /= bases[num_base];
		// update the num LCD
		UpdateNumLcd();

		// update the RGB LED for the current operand
		UpdateNumRgbLed();
		// disable red LED for last result, user wants to use what's left
		rgb_is_red.fields.result = 0;
	}
}

void Calculator_Process(void)
{
	// Process changes to the operator, numerical base, or clear/backspace
	ProcessOperator();
	ProcessNumBase();
	ProcessClearBackspace();

	// Process new input
	if (Input_GetNewBtn(BTN_C_BIT)) {
		// User submitted an operand
		if (num_idx == 0) {
			// User submitted first operand, switch to second and update LCD
			++num_idx;
			UpdateNumLcd();

			// disable red LED for last result, user wants to use what's left
			rgb_is_red.fields.result = 0;
		} else {
			// User submitted second operand, run the operation
			RunOp();
		}
	} else if (Input_IsNewKey()) {
		// User submitted another digit
		int8_t const key = Input_GetKey();
		if (key >= 0) {
			// Valid key, update the current operand
			UpdateNum(key);

			// disable red LED for last result, user wants to use what's left
			rgb_is_red.fields.result = 0;
		}
	}

	// Update the LCD output
	for (int i = 0; i < sizeof(update_lcd) / sizeof(*update_lcd); ++i) {
		if (update_lcd[i]) {
			LCD_WriteStringAtPos(lcd[i], i, 0);
		}
	}

	// Update the RGB LED
	uint8_t const is_red = !!(rgb_is_red.is_red);
	if (is_red != last_rgb_is_red) {
		RGBLED_SetValue(0x1F * is_red, 0, 0);
	}
	last_rgb_is_red = is_red;
}

/**
 * Runs the arithmetic operation on the inputs.
 */
static void RunOp(void)
{
	uint32_t num = 0;
	uint8_t div_0_err = 0;

	switch (operator) {
		case Add:
			num = nums[0] + nums[1];
			break;
		case Sub:
			num = nums[0] - nums[1];
			break;
		case Mult:
			num = nums[0] * nums[1];
			break;
		case Div:
			if (nums[1] == 0) {
				div_0_err = 1;
			} else {
				num = nums[0] / nums[1];
			}
			break;
		case And:
			num = nums[0] & nums[1];
			break;
		case Or:
			num = nums[0] | nums[1];
			break;
		case Xor:
			num = nums[0] ^ nums[1];
			break;
	}

	ResetNums();
	ClearLcd();

	if (div_0_err) {
		strcpy(lcd[0], "Err: div by 0");
		LCD_WriteStringAtPos(lcd[0], 0, 0);
	} else {
		nums[0] = num;
		UpdateNumLcd();
		
		if (num > 0xFFFF || (num_base == Bin && (num & 0x8000))) {
			rgb_is_red.fields.result = 1;
		} else {
			rgb_is_red.fields.result = 0;
		}
	}
}

/** Updates an operand based on the given keypress. */
static void UpdateNum(uint8_t key)
{
	switch (num_base) {
		case Bin:
			// key can be 0 or 1; do not accept key if bits 14 or 15 are set (max int)
			if (key <= 0b1 && !(nums[num_idx] & 0xC000)) {
				// shift left for new digit
				nums[num_idx] <<= 1;
				// add new digit
				nums[num_idx] += key;
				// update the LCD
				UpdateNumLcd();
			}
			break;
			
		case Dec:
			// key can be 0-9
			if (key <= 9) {
				// multiply operand by 10 and add new digit
				uint32_t const num = nums[num_idx] * 10 + key;
				// only update the number if it fits in 16 bits
				if (num <= 0xFFFF) {
					nums[num_idx] = num;
					// update the LCD
					UpdateNumLcd();
				}
			}
			break;
			
		case Hex:
			// key can be 0-F; do not accept key if upper byte is non-zero (max int)
			if (key <= 0xF && !(nums[num_idx] & 0xF000)) {
				// shift left for new digit
				nums[num_idx] <<= 4;
				// add new digit
				nums[num_idx] += key;
				// update the LCD
				UpdateNumLcd();
			}
			break;
	}
}

/** Updates the current operand on the LCD. */
static void UpdateNumLcd(void)
{
	NumToStr(nums[num_idx], lcd[num_idx] + 1, sizeof(lcd[num_idx]) - 2);
	update_lcd[num_idx] = 1;
}

/** Updates the current operand's RGB LED. */
static void UpdateNumRgbLed(void)
{
	if (num_base == Bin) {
		// We're in binary; if beyond 15 bits, signal overflow
		rgb_is_red.fields.num1 = !!(nums[0] & 0x8000);
		rgb_is_red.fields.num2 = !!(nums[1] & 0x8000);
	} else {
		// Not in binary, so operands cannot overflow
		rgb_is_red.fields.num1 = 0;
		rgb_is_red.fields.num2 = 0;
	}
}

/** Converts a binary number to a string. */
static void BinToStr(uint16_t bin, char *str, size_t strlen)
{
	// 16-bit binary number is max 15 digits (+ 1 blank on LCD), unless string isn't long enough
	size_t const max_len = strlen < 16 ? strlen : 15;

	memset(str, ' ', strlen);
	// no space for prefix
	for (int i = 0; i < max_len; ++i) {
		uint8_t const bin_digit = bin & 0x1;
		// set the digit i from the right of the string
		str[max_len - i - 1] = '0' + bin_digit;
		// shift out the digit we just set
		bin >>= 1;
		
		if (!bin) {
			break;
		}
	}
}

/** Converts a decimal number to a string. */
static void DecToStr(uint16_t dec, char *str, size_t strlen)
{
	// 16-bit decimal number is max 5 digits + prefix of 2, unless string isn't long enough 
	size_t const max_len = strlen < 7 ? strlen : 7;

	memset(str, ' ', strlen);
	// prefix of "0d"
	str[0] = '0';
	str[1] = 'd';
	for (int i = 0; i < max_len - 2; ++i) {
		uint8_t const dec_digit = dec % 10;
		// set the digit i from the right of the string
		str[max_len - i - 1] = '0' + dec_digit;
		// shift out the digit we just set
		dec /= 10;
		
		if (!dec) {
			break;
		}
	}
}

/** Converts a hexadecimal number to a string. */
static void HexToStr(uint16_t hex, char *str, size_t strlen)
{
	// 16-bit hex number is max 4 digits + prefix of 2, unless string isn't long enough 
	size_t const max_len = strlen < 6 ? strlen : 6;

	memset(str, ' ', strlen);
	// prefix of "0x"
	str[0] = '0';
	str[1] = 'x';
	for (int i = 0; i < max_len - 2; ++i) {
		uint8_t const hex_digit = hex & 0xF;
		// set the digit i from the right of the string
		if (hex_digit >= 0xA) {
			str[max_len - i - 1] = 'A' + hex_digit - 0xA;
		} else {
			str[max_len - i - 1] = '0' + hex_digit;
		}
		// shift out the digit we just set
		hex >>= 4;
		
		if (!hex) {
			break;
		}
	}
}

/** Converts a number to a string in the appropriate numerical base format. */
static void NumToStr(uint16_t num, char *str, size_t strlen)
{
	// we need to have enough space for a 2-digit prefix
	if (strlen < 2) return;

	switch (num_base) {
		case Bin:
			BinToStr(num, str, strlen);
			break;
		case Dec:
			DecToStr(num, str, strlen);
			break;
		case Hex:
			HexToStr(num, str, strlen);
			break;
	}
}
