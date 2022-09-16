/*
 * Module to handle user input.
 *
 * Author: Benjamin Hall
 * Date: 2022 September 16
 */

#include "input.h"
#include "peripherals/btn.h"
#include "peripherals/keypad.h"
#include "peripherals/swt.h"

static int8_t key;
static int8_t last_key;

static uint8_t btn;
static uint8_t last_btn;

static uint8_t swt;
static uint8_t last_swt;

void Input_Init(void)
{
	// init necessary peripherals
	BTN_Init();
	Keypad_Init();
	SWT_Init();

	// -1 is no key, 0 is no buttons/switches
	key = -1;
	last_key = -1;
	
	btn = 0;
	last_btn = 0;
	
	swt = 0;
	last_swt = 0;
}

void Input_Process(void)
{
	// update the previously read key/btn/swt
	last_key = key;
	last_btn = btn;
	last_swt = swt;
	// fetch the current key/btn/swt
	key = Keypad_GetKey();
	btn = BTN_GetGroupValue();
	swt = SWT_GetGroupValue();
}

uint8_t Input_IsNewKey(void)
{
	return key != last_key;
}
uint8_t Input_GetKey(void)
{
	return key;
}

uint8_t Input_GetBtn(uint8_t btn_num)
{
	// return whether the given button is pressed by applying a mask
	return !!(btn & (1 << btn_num));
}
uint8_t Input_GetNewBtn(uint8_t btn_num)
{
	uint8_t const mask = 1 << btn_num;
	// return whether the given button is pressed AND last was not
	return (btn & mask) && !(last_btn & mask);
}
uint8_t Input_IsNewBtnGroup(void)
{
	return btn != last_btn;
}
uint8_t Input_GetBtnGroup(void)
{
	return btn;
}

uint8_t Input_GetSwt(uint8_t swt_num)
{
	// return whether the given switch is toggled by applying a mask
	return !!(swt & (1 << swt_num));
}
uint8_t Input_GetNewSwt(uint8_t swt_num)
{
	uint8_t const mask = 1 << swt_num;
	// return whether the given switch is toggled AND last was not
	return (swt & mask) && !(last_swt & mask);
}
uint8_t Input_IsNewSwtGroup(void)
{
	return swt != last_swt;
}
uint8_t Input_GetSwtGroup(void)
{
	return swt;
}
