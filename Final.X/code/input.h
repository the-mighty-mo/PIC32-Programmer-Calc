#pragma once

#include <stdint.h>

/**
 * Initializes the Input module.
 * 
 * This module manages buttons, switches, and the keypad.
 */
void Input_Init(void);
/**
 * Processes input, updating keys, buttons, and switches.
 */
void Input_Process(void);

/**
 * Returns whether the pressed key has changed.
 */
uint8_t Input_IsNewKey(void);
/**
 * Returns the currently pressed key.
 */
uint8_t Input_GetKey(void);

/**
 * Returns whether the given button is pressed.
 */
uint8_t Input_GetBtn(uint8_t btn_num);
/**
 * Returns whether the given button is newly pressed (rising edge).
 */
uint8_t Input_GetNewBtn(uint8_t btn_num);
/**
 * Returns whether the pressed buttons has changed.
 */
uint8_t Input_IsNewBtnGroup(void);
/**
 * Returns bits representing all the pressed buttons.
 * Each bit represents a button, where 1 means on.
 */
uint8_t Input_GetBtnGroup(void);

/**
 * Returns whether the given switch is toggled.
 */
uint8_t Input_GetSwt(uint8_t swt_num);
/**
 * Returns whether the given switch is newly toggled (rising edge).
 */
uint8_t Input_GetNewSwt(uint8_t swt_num);
/**
 * Returns whether the toggled switches has changed.
 */
uint8_t Input_IsNewSwtGroup(void);
/**
 * Returns bits representing all the toggled switches.
 * Each bit represents a switch, where 1 means on.
 */
uint8_t Input_GetSwtGroup(void);
