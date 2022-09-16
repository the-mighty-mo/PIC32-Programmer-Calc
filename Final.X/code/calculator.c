/*
 * Module to run the programmer calculator.
 *
 * Author: Benjamin Hall
 * Date: 2022 September 16
 */

#include "calculator.h"
#include "peripherals/btn.h"
#include "peripherals/lcd.h"
#include "peripherals/led.h"
#include "input.h"
#include "utils.h"
#include "output.h"
#include <string.h>

// Operands
static uint16_t nums[2];
static uint8_t num_updated[2];
static uint8_t num_idx = 0;

// Stores whether the last result was an error
static uint8_t is_err;

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

// operator characters for display
static char const operators[] = {'+', '-', '*', '/', '&', '|', '^'};

// RGBLED output
static union {
	// whether any operation has overflowed
	uint8_t is_ovf;
	// a bitfield of operations that can overflow
	struct OverflowStatus {
		// first operand
		uint8_t num1 : 1;
		// second operand
		uint8_t num2 : 1;
		// last result
		uint8_t result : 1;
	} fields;
} overflow_stat = {0};

// Private functions
static void ProcessKey(uint8_t key);
static void RunOp(void);
static void UpdateOvfStats(void);
static void WriteNumLcd(uint8_t idx);
static void NumToStr(uint16_t num, char *str, size_t strlen);

/** Resets the operands and operator. */
static void ResetNums(void)
{
	memset(nums, 0, sizeof(nums));
	memset(num_updated, 0, sizeof(num_updated));
	num_idx = 0;
	is_err = 0;
	memset(&overflow_stat, 0, sizeof(overflow_stat));
}

/** Resets the LCD output. */
static void ResetLcd(void)
{
	char* lcd[] = {Output_GetLcdBuffer(0), Output_GetLcdBuffer(1)};

	// first line: first operand
	NumToStr(nums[0], lcd[0] + 1, LCD_BUFFER_STRLEN - 1);
	lcd[0][0] = ' ';

	// second line: operator and spaces
	memset(lcd[1], ' ', LCD_BUFFER_STRLEN);
	lcd[1][0] = operators[operator];

	// signal that we need to write to the LCD
	Output_SignalLcdUpdate(0);
	Output_SignalLcdUpdate(1);
}

void Calculator_Init(void)
{
	// reset the operands and operator
	ResetNums();
	num_base = Hex;
	operator = Add;
	// clear the overflow status
	memset(&overflow_stat, 0, sizeof(overflow_stat));
	// clear the LCD display
	ResetLcd();
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
				Output_GetLcdBuffer(1)[0] = operators[operator];
				Output_SignalLcdUpdate(1);
				break;
			}
		}
	}
	// set the LED of the active operator
	LED_SetGroupValue(1 << operator);
}

/** Updates the numerical base used for output. */
static void UpdateNumBase(void)
{
	// signal that we need to update the displayed number
	num_updated[0] = 1;
	// also signal to update the second operand if we're on it
	if (num_idx) {
		num_updated[1] = 1;
	}

	// update the overflow status for the current operand
	UpdateOvfStats();
}

/**
 * Reads the buttons and updates the numerical base.
 */
static void ProcessNumBase(void)
{
	if (Input_GetNewBtn(BTN_U_BIT)) {
		// user wants to go up a base
		if (++num_base > Hex) {
			// wrap to binary
			num_base = Bin;
		}
		// update the base used for output
		UpdateNumBase();
	} else if (Input_GetNewBtn(BTN_D_BIT)) {
		// user wants to go up a base
		if (num_base-- == 0) {
			// wrap to hex
			num_base = Hex;
		}
		// update the base used for output
		UpdateNumBase();
	}
}

/**
 * Reads the buttons and clears or deletes characters from the input.
 */
static void ProcessClearBackspace(void)
{
	if (Input_GetNewBtn(BTN_R_BIT)) {
		// user wants to clear the input
		if (nums[num_idx] != 0) {
			// clear the current operand
			nums[num_idx] = 0;
			num_updated[num_idx] = 1;

			// update the overflow status for the current operand
			UpdateOvfStats();
			// disable red LED for last result, user wants to use what's left
			overflow_stat.fields.result = 0;
		} else {
			// clear all operands
			ResetNums();
			ResetLcd();
		}
	} else if (Input_GetNewBtn(BTN_L_BIT) && nums[num_idx]) {
		// shift out the most recent digit (least significant))
		nums[num_idx] /= bases[num_base];
		// signal to update the num output
		num_updated[num_idx] = 1;

		// update the overflow status for the current operand
		UpdateOvfStats();
		// disable red LED for last result, user wants to use what's left
		overflow_stat.fields.result = 0;
	}
}

/** Checks for the clear button to clear the current error. */
static uint8_t CheckForClear(void)
{
	if (Input_GetNewBtn(BTN_R_BIT)) {
		// clear the error status
		is_err = 0;

		// reset the operands and clear the screen
		ResetNums();
		ResetLcd();

		// signal that we cleared
		return 1;
	} else {
		// did not clear
		return 0;
	}
}

void Calculator_Process(void)
{
	// process changes to the operator
	ProcessOperator();

	if (is_err) {
		// last operation was an error, check for clear
		if (!CheckForClear()) {
			// user hasn't cleared yet, leave early
			return;
		}
	}

	// process changes to the numerical base or clear/backspace
	ProcessNumBase();
	ProcessClearBackspace();

	// process new input
	if (Input_GetNewBtn(BTN_C_BIT)) {
		// user submitted an operand
		if (num_idx == 0) {
			// user submitted first operand, switch to second and update output
			++num_idx;
			num_updated[num_idx] = 1;

			// disable red LED for last result, user wants to use what's left
			overflow_stat.fields.result = 0;
		} else {
			// user submitted second operand, run the operation
			RunOp();
		}
	} else if (Input_IsNewKey()) {
		// user submitted another digit
		int8_t const key = Input_GetKey();
		if (key >= 0) {
			// valid key, update the current operand
			ProcessKey(key);

			// disable red LED for last result, user wants to use what's left
			overflow_stat.fields.result = 0;
		}
	}

	// update the LCD output
	if (num_updated[0]) {
		WriteNumLcd(0);
	}
	if (num_updated[1]) {
		WriteNumLcd(1);
	}

	// update the RGB LED
	uint8_t const is_red = !!(overflow_stat.is_ovf);
	Output_SetRgbColor(0x1F * is_red, 0, 0);
}

/** Updates an operand based on the given keypress. */
static void ProcessKey(uint8_t key)
{
	switch (num_base) {
		case Bin:
			// key can be 0 or 1; do not accept key if bits 14 or 15 are set (max int)
			if (key <= 0b1 && !(nums[num_idx] & 0xC000)) {
				// shift left for new digit
				nums[num_idx] <<= 1;
				// add new digit
				nums[num_idx] += key;
				// update the num output
				num_updated[num_idx] = 1;
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
					// update the num output
					num_updated[num_idx] = 1;
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
				// update the num output
				num_updated[num_idx] = 1;
			}
			break;
	}
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
			// check for divide by 0
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

	// reset operands and clear screen
	ResetNums();
	ResetLcd();

	// set output
	if (div_0_err) {
		// output an error to the LCD
		strcpy(Output_GetLcdBuffer(0), "Err: div by 0");
		Output_SignalLcdUpdate(0);
		// signal an error
		is_err = 1;
	} else {
		// output the result
		nums[0] = num;
		num_updated[0] = 1;

		// set the overflow status
		if (num > 0xFFFF || (num_base == Bin && (num & 0x8000))) {
			overflow_stat.fields.result = 1;
		} else {
			overflow_stat.fields.result = 0;
		}
	}
}

/** Updates the current operand's overflow status. */
static void UpdateOvfStats(void)
{
	if (num_base == Bin) {
		// We're in binary; if beyond 15 bits, signal overflow
		overflow_stat.fields.num1 = !!(nums[0] & 0x8000);
		overflow_stat.fields.num2 = !!(nums[1] & 0x8000);
	} else {
		// Not in binary, so operands cannot overflow
		overflow_stat.fields.num1 = 0;
		overflow_stat.fields.num2 = 0;
	}
}

/** Writes the given operand onto the LCD. */
static void WriteNumLcd(uint8_t idx)
{
	char *lcd = Output_GetLcdBuffer(idx);
	// convert the operand to a string
	NumToStr(nums[idx], lcd + 1, LCD_BUFFER_STRLEN - 1);
	// signal that we want to update this line of the LCD
	Output_SignalLcdUpdate(idx);
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
