#include "config.h"
#include "peripherals/btn.h"
#include "peripherals/keypad.h"
#include "peripherals/swt.h"
#include "peripherals/lcd.h"
#include "peripherals/led.h"
#include "peripherals/rgbled.h"
#include "input.h"
#include "utils.h"

static void App_Init()
{
	// JTAG conflicts with LEDs
	CFGCONbits.JTAGEN = 0;

	BTN_Init();
	Keypad_Init();
	SWT_Init();
	LCD_Init();
	LED_Init();
	RGBLED_Init();
	Input_Init();
}

static void App_Process()
{
	Keypad_Process();
	Input_Process();
}

int main()
{
	App_Init();

	while (1) {
		App_Process();
		DelayAprox100Us(10);
	}
}
