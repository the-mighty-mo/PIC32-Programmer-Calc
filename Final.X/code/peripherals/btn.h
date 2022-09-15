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

/* *****************************************************************************
 End of File
 */
