/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Digilent

  @File Name
    btn.h

  @Description
        This file groups the declarations of the functions that implement
        the BTN library (defined in btn.c).
        Include the file in the project when this library is needed.
        Use #include "btn.h" in the source files where the functions are needed.
 */
/* ************************************************************************** */

#pragma once

void BTN_Init();
unsigned char BTN_GetValue(unsigned char btn);
unsigned char BTN_GetGroupValue();

// Button defines
#define BTN_U_BIT 0
#define BTN_U_MASK (1 << BTN_U_BIT)
#define BTN_L_BIT 1
#define BTN_L_MASK (1 << BTN_L_BIT)
#define BTN_C_BIT 2
#define BTN_C_MASK (1 << BTN_C_BIT)
#define BTN_R_BIT 3
#define BTN_R_MASK (1 << BTN_R_BIT)
#define BTN_D_BIT 4
#define BTN_D_MASK (1 << BTN_D_BIT)

/* *****************************************************************************
 End of File
 */
