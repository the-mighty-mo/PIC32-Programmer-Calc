/* ************************************************************************** */
/** Descriptive File Name
  @Company
    Digilent
  @File Name
    utils.h
  @Description
    This file groups the declarations of the functions that implement
        the utils library (defined in utils.c).
        Include the file in the project when this library is needed.
        Use #include "utils.h" in the source files where the functions are needed.
 */
/* ************************************************************************** */

#pragma once

#include <stdlib.h>
#include <string.h>

void DelayAprox100Us( unsigned int tusDelay );
void HexToStr(int hex, char *str, size_t strlen);

/* *****************************************************************************
 End of File
 */