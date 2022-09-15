// PIC32MX370F512L Configuration Bit Settings

// DEVCFG2
#pragma config FPLLIDIV = DIV_2        // PLL Input Divider (12x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (24x Multiplier)
#pragma config FPLLODIV = DIV_1       // System PLL Output Clock Divider (PLL Divide by 256)

// DEVCFG1
#pragma config FNOSC = FRCPLL           // Oscillator Selection Bits (Fast RC Osc w/Div-by-N (FRCDIV))
#pragma config FSOSCEN = OFF             // Secondary Oscillator Enable (Enabled)
#pragma config POSCMOD = XT            // Primary Oscillator Configuration (Primary osc disabled)
#pragma config OSCIOFNC = ON           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_2           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/8)
#pragma config FWDTEN = OFF              // Watchdog Timer Enable (WDT Disabled)

// DEVCFG0
#pragma config JTAGEN = OFF             // JTAG Enable (JTAG Disabled)

#include "config.h"
#include "peripherals/btn.h"
#include "peripherals/keypad.h"
#include "peripherals/swt.h"
#include "peripherals/lcd.h"
#include "peripherals/led.h"
#include "peripherals/rgbled.h"
#include "calculator.h"
#include "input.h"
#include "utils.h"

static void App_Init()
{
	// Initialize all the peripherals
	BTN_Init();
	Keypad_Init();
	SWT_Init();
	LCD_Init();
	LED_Init();
	RGBLED_Init();
	// Initialize the input module
	Input_Init();
	// Initialize the calculator module
	Calculator_Init();
}

static void App_Process()
{
	// Process inputs
	Input_Process();
	// Process the calculator
	Calculator_Process();
}

int main()
{
	// Initialize modules
	App_Init();

	while (1) {
		// Run the modules
		App_Process();
		// 1 ms delay
		DelayAprox100Us(10);
	}
}
