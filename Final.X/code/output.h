/*
 * Module to handle program output.
 *
 * Author: Benjamin Hall
 * Date: 2022 September 16
 */

#pragma once

#include <stdint.h>

#define LCD_BUFFER_STRLEN 16
#define LCD_BUFFER_COUNT 2

/**
 * Initializes the Output module.
 * 
 * This module manages the LCD and RGB LED.
 */
void Output_Init(void);
/**
 * Processes output, applying updates to the LCD and RGB LED.
 */
void Output_Process(void);

/**
 * Returns a pointer to the LCD buffer for the given line index.
 */
char *Output_GetLcdBuffer(uint8_t idxLine);
/**
 * Signals that the LCD buffer for the given line index should be updated.
 */
void Output_SignalLcdUpdate(uint8_t idxLine);

/**
 * Sets the color of the RGB LED.
 */
void Output_SetRgbColor(uint8_t r, uint8_t g, uint8_t b);
