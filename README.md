# PIC32-Programmer-Calc

Hardware implementation of the Windows 10/11 programmer calculator for the Basys MX3 board using the PIC32.

Switches determine the operation:
- Add (+)
- Subtract (-)
- Multiply (*)
- Divide (/)
- And (&)
- Or (|)
- Xor (^)

The U and D buttons switch the number format between binary, decimal, and hexadecimal.
The C button submits an operand.
The R button is the clear button. If the current operand is non-zero, it clears the current operand. Otherwise, it clears all input.
The L button is the backspace button.

A PmodKYPD is used to input digits.

The RGB LED is set to red on overflow. Overflow can happen after an operation, or when switching to binary when an operand exceeds 15 bits --
input in other modes is 16-bit, but the LCD is only wide enough to show 15 digits plus the operator.
