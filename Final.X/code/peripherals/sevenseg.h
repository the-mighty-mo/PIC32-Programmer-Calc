/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Digilent

  @File Name
    ssd.h

  @Description
    This file groups the declarations of the functions that implement
        the SSD library (defined in ssd.c).
        Include the file in the project when this library is needed.
        Use #include "ssd.h" in the source files where the functions are needed.
 */
/* ************************************************************************** */

#pragma once

    // *****************************************************************************
    // *****************************************************************************
    // Section: Interface Functions
    // *****************************************************************************
    // *****************************************************************************

void SevenSeg_Init();
void SevenSeg_WriteDigits(unsigned char d0, unsigned char d1, unsigned char d2, unsigned char d3, \
            unsigned char dp1, unsigned char dp2, unsigned char dp3, unsigned char dp4);
void SevenSeg_WriteDigitsGrouped(unsigned int val, unsigned char dp);
unsigned char SSD_GetDigitSegments(unsigned char d);

/* *****************************************************************************
 End of File
 */
