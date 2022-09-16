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

// TODO: clean this up
static void UpdateBase(void);
static void NumToStr(uint16_t num, char *str, size_t strlen);
static void UpdateLcd(void);
static void UpdateNum(uint8_t key);

static void RunOp(void);

/** Resets the operands and operator. */
static void ResetNums(void)
{
	memset(nums, 0, sizeof(nums));
	num_idx = 0;
	RGBLED_SetValue(0, 0, 0);
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

void Calculator_Init(void)
{
	// Reset the operands and operator
	ResetNums();
	num_base = Hex;
	operator = Add;
	// Clear the LCD display
	ClearLcd();
}

/**
 * Reads the switches and updates the current operator.
 */
static void ProcessOperator(void)
{
	uint8_t const swt = Input_GetSwtGroup();
	if (Input_IsNewSwtGroup() && (swt & (swt - 1)) == 0) {
		// only one switch is set, set operation
		for (int i = 0; i < sizeof(operators) / sizeof(*operators); ++i) {
			if (swt & (1 << i)) {
				operator = i;
				lcd[1][0] = operators[operator];
				update_lcd[1] = 1;
				break;
			}
		}
	}
	LED_SetGroupValue(1 << operator);
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
		if (nums[num_idx] == 0) {
			ResetNums();
			ClearLcd();
		} else {
			nums[num_idx] = 0;
			UpdateLcd();
		}
	} else if (Input_GetNewBtn(BTN_L_BIT) && nums[num_idx]) {
		nums[num_idx] /= bases[num_base];
		UpdateLcd();
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
			UpdateLcd();
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
		}
	}

	// Update the LCD output
	for (int i = 0; i < sizeof(update_lcd) / sizeof(*update_lcd); ++i) {
		if (update_lcd[i]) {
			LCD_WriteStringAtPos(lcd[i], i, 0);
		}
	}
}

static void UpdateNum(uint8_t key)
{
	switch (num_base) {
		case Bin:
			if (key <= 0b1 && !(nums[num_idx] & 0xC000)) {
				nums[num_idx] <<= 1;
				nums[num_idx] += key;
				UpdateLcd();
			}
			break;
			
		case Dec:
			if (key <= 9) {
				uint32_t const num = nums[num_idx] * 10 + key;
				if (num <= 0xFFFF) {
					nums[num_idx] = num;
					UpdateLcd();
				}
			}
			break;
			
		case Hex:
			if (key <= 0xF && !(nums[num_idx] & 0xF000)) {
				nums[num_idx] <<= 4;
				nums[num_idx] += key;
				UpdateLcd();
			}
			break;
	}
}

static void UpdateLcd(void)
{
	NumToStr(nums[num_idx], lcd[num_idx] + 1, sizeof(lcd[num_idx]) - 2);
	update_lcd[num_idx] = 1;
//	LCD_WriteStringAtPos(lcd[num_idx], num_idx, 0);
	RGBLED_SetValue(0, 0, 0);
}

static void UpdateBase(void)
{
	NumToStr(nums[0], lcd[0] + 1, sizeof(lcd[0]) - 2);
	update_lcd[0] = 1;
//	LCD_WriteStringAtPos(lcd[0], 0, 0);
	
	if (num_idx) {
		NumToStr(nums[1], lcd[1] + 1, sizeof(lcd[1]) - 2);
		update_lcd[1] = 1;
//		LCD_WriteStringAtPos(lcd[1], 1, 0);
	}
	
	if (num_base == Bin && ((nums[0] & 0x8000) || (nums[1] & 0x8000))) {
		RGBLED_SetValue(0x1F, 0x00, 0x00);
	} else {
		RGBLED_SetValue(0, 0, 0);
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
	
	ClearLcd();
	ResetNums();
	
	if (div_0_err) {
		strcpy(lcd[0], "Err: div by 0");
		LCD_WriteStringAtPos(lcd[0], 0, 0);
	} else {
		nums[0] = num;
		UpdateLcd();
		
		if (num > 0xFFFF || (num_base == Bin && (num & 0x8000))) {
			RGBLED_SetValue(0x1F, 0x00, 0x00);
		} else {
			RGBLED_SetValue(0, 0, 0);
		}
	}
}

static void BinToStr(uint16_t bin, char *str, size_t strlen)
{
	size_t const max_len = strlen < 16 ? strlen : 15;
	memset(str, ' ', strlen);
	for (int i = 0; i < max_len; ++i) {
		uint8_t const bin_digit = bin & 0x1;
		str[max_len - i - 1] = '0' + bin_digit;
		bin >>= 1;
		
		if (!bin) {
			break;
		}
	}
}

static void DecToStr(uint16_t dec, char *str, size_t strlen)
{
	size_t const max_len = strlen < 7 ? strlen : 7;
	memset(str, ' ', strlen);
	str[0] = '0';
	str[1] = 'd';
	for (int i = 0; i < max_len - 2; ++i) {
		uint8_t const dec_digit = dec % 10;
		str[max_len - i - 1] = '0' + dec_digit;
		dec /= 10;
		
		if (!dec) {
			break;
		}
	}
}

static void HexToStr(uint16_t hex, char *str, size_t strlen)
{
	size_t const max_len = strlen < 6 ? strlen : 6;
	memset(str, ' ', strlen);
	str[0] = '0';
	str[1] = 'x';
	for (int i = 0; i < max_len - 2; ++i) {
		uint8_t const hex_digit = hex & 0xF;
		if (hex_digit >= 0xA) {
			str[max_len - i - 1] = 'A' + hex_digit - 0xA;
		} else {
			str[max_len - i - 1] = '0' + hex_digit;
		}
		hex >>= 4;
		
		if (!hex) {
			break;
		}
	}
}

static void NumToStr(uint16_t num, char *str, size_t strlen)
{
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
