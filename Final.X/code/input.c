#include "input.h"
#include "peripherals/btn.h"
#include "peripherals/keypad.h"
#include "peripherals/lcd.h"
#include "peripherals/led.h"
#include "peripherals/rgbled.h"
#include "utils.h"
#include "peripherals/swt.h"
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
	Xor
} operator;

static uint8_t btn;
static uint8_t last_btn;

static uint8_t swt;
static uint8_t last_swt;

#define GET_BTN(mask) ((btn & mask) && !(last_btn & mask))

static char lcd[2][17];
static char const operators[] = {'+', '-', '*', '/', '&', '|', '^'};

static void UpdateBase(void);
static void NumToStr(uint16_t num, char *str, size_t strlen);
static void UpdateLcd(void);
static void UpdateNum(uint8_t key);

static void RunOp(void);

static void ResetNums(void)
{
	memset(nums, 0, sizeof(nums));
	num_idx = 0;
	RGBLED_SetValue(0, 0, 0);
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
	
	swt = 0;
	last_swt = 0;
	
	ClearLcd();
}

void Input_Process(void)
{
	last_btn = btn;
	btn = BTN_GetGroupValue();

	last_swt = swt;
	swt = SWT_GetGroupValue();
	if (swt != last_swt && (swt & (swt - 1)) == 0) {
		// only one switch is set, set operation
		for (int i = 0; i < sizeof(operators) / sizeof(*operators); ++i) {
			if (swt & (1 << i)) {
				operator = i;
				lcd[1][0] = operators[operator];
				LCD_WriteStringAtPos(lcd[1], 1, 0);
				break;
			}
		}
	}
	LED_SetGroupValue(1 << operator);

	if (GET_BTN(BTN_U_MASK)) {
		if (++num_base > Hex) {
			num_base = Bin;
		}
		UpdateBase();
	} else if (GET_BTN(BTN_D_MASK)) {
		if (num_base-- == 0) {
			num_base = Hex;
		}
		UpdateBase();
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
			RunOp();
		}
	} else if (Keypad_IsNewKey()) {
		int8_t const key = Keypad_GetKey();
		if (key >= 0) {
			UpdateNum(key);
		}
	}
}

static void UpdateNum(uint8_t key)
{
	switch (num_base) {
		case Bin:
			if (key <= 0b1 && !(nums[num_idx] & 0x4000)) {
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
	LCD_WriteStringAtPos(lcd[num_idx], num_idx, 0);
	RGBLED_SetValue(0, 0, 0);
}

static void UpdateBase(void)
{
	NumToStr(nums[0], lcd[0] + 1, sizeof(lcd[0]) - 2);
	LCD_WriteStringAtPos(lcd[0], 0, 0);
	
	if (num_idx) {
		NumToStr(nums[1], lcd[1] + 1, sizeof(lcd[1]) - 2);
		LCD_WriteStringAtPos(lcd[1], 1, 0);
	}
	
	if (num_base == Bin && ((nums[0] & 0x8000) || (nums[1] & 0x8000))) {
		RGBLED_SetValue(0x1F, 0x00, 0x00);
	} else {
		RGBLED_SetValue(0, 0, 0);
	}
}

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
	memset(str, ' ', strlen);
	for (int i = 0; i < 15; ++i) {
		uint8_t const bin_digit = bin & 0x1;
		str[15 - i - 1] = '0' + bin_digit;
		bin >>= 1;
		
		if (!bin) {
			break;
		}
	}
}

static void DecToStr(uint16_t dec, char *str, size_t strlen)
{
	memset(str, ' ', strlen);
	for (int i = 0; i < 5; ++i) {
		uint8_t const dec_digit = dec % 10;
		str[5 - i - 1] = '0' + dec_digit;
		dec /= 10;
		
		if (!dec) {
			break;
		}
	}
}

static void HexToStr(uint16_t hex, char *str, size_t strlen)
{
	memset(str, ' ', strlen);
	for (int i = 0; i < 4; ++i) {
		uint8_t const hex_digit = hex & 0xF;
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

static void NumToStr(uint16_t num, char *str, size_t strlen)
{
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
