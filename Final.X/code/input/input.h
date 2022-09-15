#pragma once

#include <stdint.h>

void Input_Init();
void Input_Process();

uint8_t Input_HasNewKey();
uint8_t Input_GetKey();
