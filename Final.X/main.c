#include "config.h"
#include "keypad.h"
#include "lcd.h"
#include "utils.h"
#include <string.h>

static void HexToStr(int hex, char *str, size_t strlen);

int main() {
	Keypad_Init();
	LCD_Init();
	LCD_DisplayClear();

	int8_t last_key = -1;
	int num = 0;

	char str[] = "                ";

	while (1) {
		int8_t const key = Keypad_GetKey();
		if (key != last_key) {
			if (key >= 0 && !(num & 0xF0000000)) {
				num <<= 4;
				num |= key;
				HexToStr(num, str, sizeof (str) - 1);
				LCD_WriteStringAtPos(str, 0, 0);
			}
			last_key = key;
		}
		DelayAprox10Us(100);
	}
}

static void HexToStr(int hex, char *str, size_t strlen) {
	int digit_cnt = 0;
	int rev_hex = 0;
	while (hex) {
		rev_hex <<= 4;
		rev_hex |= hex & 0xF;
		hex >>= 4;
		++digit_cnt;
	}

	int i;
	for (i = 0; i < strlen && digit_cnt; ++i, --digit_cnt) {
		uint8_t const hex_digit = rev_hex & 0xF;
		if (hex_digit >= 0xA) {
			str[i] = 'A' + hex_digit - 0xA;
		} else {
			str[i] = '0' + hex_digit;
		}
		rev_hex >>= 4;
	}
	memset(str + i, ' ', strlen - i);
}
