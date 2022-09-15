#include "input/input.h"
#include "input/keypad.h"

static int8_t key = 0;
static int8_t last_key = -1;
static uint8_t is_new_key = 0;

void Input_Init()
{
	Keypad_Init();
}

void Input_Process()
{
	key = Keypad_GetKey();
	is_new_key = key != last_key;
	last_key = key;
}

uint8_t Input_HasNewKey()
{
	return is_new_key;
}
uint8_t Input_GetKey()
{
	return key;
}
