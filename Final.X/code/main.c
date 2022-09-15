#include "config.h"
#include "peripherals/btn.h"
#include "peripherals/keypad.h"
#include "peripherals/swt.h"
#include "peripherals/lcd.h"
#include "utils.h"

static int num = 0;
static char str[] = "                ";

static void App_Init()
{
	BTN_Init();
	Keypad_Init();
	SWT_Init();
	LCD_Init();
	LCD_DisplayClear();
}

static void App_Process()
{
	Keypad_Process();
	if (Keypad_IsNewKey()) {
		int8_t const key = Keypad_GetKey();
		if (key >= 0 && !(num & 0xF0000000)) {
			num <<= 4;
			num |= key;
			HexToStr(num, str, sizeof (str) - 1);
			LCD_WriteStringAtPos(str, 0, 0);
		}
	}
}

int main()
{
	App_Init();

	while (1) {
		App_Process();
		DelayAprox100Us(10);
	}
}
