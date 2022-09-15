#include "config.h"
#include "keypad.h"
#include "lcd.h"
#include "utils.h"

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
		DelayAprox100Us(10);
	}
}
