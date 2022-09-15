/* ************************************************************************** */
/** Descriptive File Name
  @Company
    Digilent
  @File Name
    utils.c
  @Description
        This library implements the delay functionality used in other libraries.  
        The delay is implemented using loop, so the delay time is not exact. 
        For exact timing use timers.
        Include the file in the project, together with utils.h, when this library is needed	
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#include <xc.h>
#include <sys/attribs.h>
#include "utils.h"
/* ************************************************************************** */

/* ------------------------------------------------------------ */
/***    Delay100Us
**
**	Synopsis:
**		Delay100Us(100)
**
**	Parameters:
**		t100usDelay - the amount of time you wish to delay in hundreds of microseconds
**
**	Return Values:
**      none
**
**	Errors:
**		none
**
**	Description:
**		This procedure delays program execution for the specified number
**      of microseconds. This delay is not precise.
**		
**	Note:
**		This routine is written with the assumption that the
**		system clock is 40 MHz.
*/
void DelayAprox100Us( unsigned int  t100usDelay )
{
    volatile int j;
    while ( 0 < t100usDelay )
    {
        t100usDelay--;
        j = 14;
        while ( 0 < j )
        {
            j--;
        }   // end while 
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
         
    }   // end while
}


void HexToStr(int hex, char *str, size_t strlen)
{
	int digit_cnt = 0;
	int rev_hex = 0;
	while (hex) {
		rev_hex <<= 4;
		rev_hex |= hex & 0xF;
		hex >>= 4;
		++digit_cnt;
	}

	int i;
	for (i = 0; i < strlen && digit_cnt; ++i, --digit_cnt) {
		uint8_t const hex_digit = rev_hex & 0xF;
		if (hex_digit >= 0xA) {
			str[i] = 'A' + hex_digit - 0xA;
		} else {
			str[i] = '0' + hex_digit;
		}
		rev_hex >>= 4;
	}
	memset(str + i, ' ', strlen - i);
}

/* *****************************************************************************
 End of File
 */
