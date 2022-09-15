#include "config.h"
#include "input/input.h"
#include "lcd.h"
#include "utils.h"

static int num = 0;
static char str[] = "                ";

static void App_Init()
{
	Input_Init();
	LCD_Init();
	LCD_DisplayClear();
}

static void App_Process()
{
	Input_Process();
	if (Input_HasNewKey()) {
		int8_t const key = Input_GetKey();
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
