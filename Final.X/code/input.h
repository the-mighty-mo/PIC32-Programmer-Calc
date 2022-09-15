#pragma once

#include <stdint.h>

void Input_Init(void);
void Input_Process(void);

uint8_t Input_IsNewKey(void);
uint8_t Input_GetKey(void);

uint8_t Input_GetBtn(uint8_t btn_num);
uint8_t Input_GetNewBtn(uint8_t btn_num);
uint8_t Input_IsNewBtnGroup(void);
uint8_t Input_GetBtnGroup(void);

uint8_t Input_GetSwt(uint8_t swt_num);
uint8_t Input_GetNewSwt(uint8_t swt_num);
uint8_t Input_IsNewSwtGroup(void);
uint8_t Input_GetSwtGroup(void);
