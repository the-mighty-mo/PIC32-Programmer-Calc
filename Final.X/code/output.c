/*
 * Module to handle program output.
 *
 * Author: Benjamin Hall
 * Date: 2022 September 16
 */

#include "output.h"
#include "peripherals/lcd.h"
#include "peripherals/rgbled.h"
#include <string.h>

static char lcd[2][17] = {0};
static uint8_t update_lcd[2] = {0};

struct RgbColor {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};
static struct RgbColor rgb_color = {0};
static struct RgbColor last_rgb_color = {0xFF, 0xFF, 0xFF};

void Output_Init(void)
{
	// init necessary peripherals
	LCD_Init();
	RGBLED_Init();

	// reset LCD string
	memset(&lcd, 0, sizeof(lcd));
	memset(&update_lcd, 0, sizeof(update_lcd));

	// reset RGB color
	memset(&rgb_color, 0, sizeof(rgb_color));
}

void Output_Process(void)
{
	// update the LCD output
	for (int i = 0; i < sizeof(update_lcd) / sizeof(*update_lcd); ++i) {
		if (update_lcd[i]) {
			// update this line of the LCD
			LCD_WriteStringAtPos(lcd[i], i, 0);
		}
		update_lcd[i] = 0;
	}

	// update the RGB LED
	if (memcmp(&rgb_color, &last_rgb_color, sizeof(rgb_color))) {
		// state changed, update RGB LED
		RGBLED_SetValue(rgb_color.r, rgb_color.g, rgb_color.b);
	}
	last_rgb_color = rgb_color;
}

char *Output_GetLcdBuffer(uint8_t idxLine)
{
	return lcd[idxLine];
}

void Output_SignalLcdUpdate(uint8_t idxLine)
{
	update_lcd[idxLine] = 1;
}

void Output_SetRgbColor(uint8_t r, uint8_t g, uint8_t b)
{
	rgb_color.r = r;
	rgb_color.g = g;
	rgb_color.b = b;
}
