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
	key = -1;
	last_key = -1;
	
	btn = 0;
	last_btn = 0;
	
	swt = 0;
	last_swt = 0;
}

void Input_Process(void)
{
	key = Keypad_GetKey();
	btn = BTN_GetGroupValue();
	swt = SWT_GetGroupValue();
}

uint8_t Input_IsNewKey(void)
{
	uint8_t const new = key != last_key;
	last_key = key;
	return new;
}
uint8_t Input_GetKey(void)
{
	return key;
}

uint8_t Input_GetBtn(uint8_t btn_num)
{
	return !!(btn & (1 << btn_num));
}
uint8_t Input_GetNewBtn(uint8_t btn_num)
{
	uint8_t const mask = 1 << btn_num;
	uint8_t const new = (btn & mask) && !(last_btn & mask);
	last_btn &= ~mask;
	last_btn |= btn & mask;
	return new;
}
uint8_t Input_IsNewBtnGroup(void)
{
	uint8_t const new = btn != last_btn;
	last_btn = btn;
	return new;
}
uint8_t Input_GetBtnGroup(void)
{
	return btn;
}

uint8_t Input_GetSwt(uint8_t swt_num)
{
	return !!(swt & (1 << swt_num));
}
uint8_t Input_GetNewSwt(uint8_t swt_num)
{
	uint8_t const mask = 1 << swt_num;
	uint8_t const new = (swt & mask) && !(last_swt & mask);
	last_swt &= ~mask;
	last_swt |= swt & mask;
	return new;
}
uint8_t Input_IsNewSwtGroup(void)
{
	uint8_t const new = swt != last_swt;
	last_swt = swt;
	return new;
}
uint8_t Input_GetSwtGroup(void)
{
	return swt;
}
