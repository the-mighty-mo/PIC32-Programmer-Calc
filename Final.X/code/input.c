#include "input.h"
#include "peripherals/btn.h"
#include "peripherals/keypad.h"
#include "peripherals/lcd.h"
#include "utils.h"
#include <string.h>

static uint16_t nums[2];
static uint8_t num_idx;

static enum NumBase {
	Bin,
	Dec,
	Hex
} num_base;
static uint8_t bases[] = {2, 10, 16};

static enum Operator {
	Add,
	Sub,
	Mult,
	Div,
	And,
	Or,
	Xor,
	Not
} operator;

static uint8_t btn;
static uint8_t last_btn;

#define GET_BTN(mask) ((btn & mask) && !(last_btn & mask))

static char lcd[2][17];
static char const operators[] = {'+', '-', '*', '/', '&', '|', '^', '~'};

static void UpdateBase(enum NumBase orig_base, enum NumBase new_base);
static void NumToStr(uint16_t num, char *str, size_t strlen);
static void UpdateLcd(void);

static void ResetNums(void)
{
	memset(nums, 0, sizeof(nums));
	num_idx = 0;
}

static void ClearLcd(void)
{
	memset(lcd[0], ' ', sizeof(lcd[0]) - 1);
	lcd[0][sizeof(lcd[0]) - 1] = '\0';
	memset(lcd[1], ' ', sizeof(lcd[1]) - 1);
	lcd[1][sizeof(lcd[1]) - 1] = '\0';
	
	NumToStr(nums[0], lcd[0] + 1, sizeof(lcd[0]) - 2);
	lcd[1][0] = operators[operator];

	LCD_WriteStringAtPos(lcd[0], 0, 0);
	LCD_WriteStringAtPos(lcd[1], 1, 0);
}

void Input_Init(void)
{
	ResetNums();
	num_base = Hex;
	operator = Add;
	
	btn = 0;
	last_btn = 0;
	
	ClearLcd();
}

void Input_Process(void)
{
	last_btn = btn;
	btn = BTN_GetGroupValue();
	
	enum NumBase const orig_base = num_base;
	if (GET_BTN(BTN_U_MASK)) {
		if (++num_base > Hex) {
			num_base = Bin;
		}
		UpdateBase(orig_base, num_base);
	} else if (GET_BTN(BTN_D_MASK)) {
		if (--num_base < 0) {
			num_base = Hex;
		}
		UpdateBase(orig_base, num_base);
	}
	
	if (GET_BTN(BTN_R_MASK)) {
		if (nums[num_idx] == 0) {
			ResetNums();
			ClearLcd();
		} else {
			nums[num_idx] = 0;
			UpdateLcd();
		}
	} else if (GET_BTN(BTN_L_MASK) && nums[num_idx]) {
		nums[num_idx] /= bases[num_base];
		UpdateLcd();
	}
	
	if (GET_BTN(BTN_C_MASK)) {
		if (num_idx == 0) {
			++num_idx;
			UpdateLcd();
		} else {
			// perform operation
		}
	} else if (Keypad_IsNewKey()) {
		int8_t const key = Keypad_GetKey();
		if (key >= 0 && !(nums[num_idx] & 0xF000)) {
			nums[num_idx] *= bases[num_base];
			nums[num_idx] += key;
			UpdateLcd();
		}
	}
}

static void UpdateLcd(void)
{
	NumToStr(nums[num_idx], lcd[num_idx] + 1, sizeof(lcd[num_idx]) - 2);
	LCD_WriteStringAtPos(lcd[num_idx], num_idx, 0);
}

static void UpdateBase(enum NumBase orig_base, enum NumBase new_base)
{
	
}

static void HexToStr(uint16_t hex, char *str, size_t strlen)
{
	memset(str, ' ', strlen);
	for (int i = 0; i < 4; ++i) {
		uint8_t hex_digit = hex & 0xF;
		if (hex_digit >= 0xA) {
			str[4 - i - 1] = 'A' + hex_digit - 0xA;
		} else {
			str[4 - i - 1] = '0' + hex_digit;
		}
		hex >>= 4;
		
		if (!hex) {
			break;
		}
	}
}

static void NumToStr(uint16_t hex, char *str, size_t strlen)
{
	HexToStr(hex, str, strlen);
}
