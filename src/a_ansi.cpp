
#include "freeCGI.h"

#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

//
// Extensions to ANSI C++
//

//
// strrev and _strrev: User to reverse a string
//
#ifndef _strrev
char * _strrev (char *pcString)
{
  char *pcStart = pcString, *pcLeft = pcString;

  while (*pcString++);                 //a_Find end of string
 
  pcString -= 2;

  //a_Do reversal
  char cX;
  while (pcLeft < pcString)
  {
    cX = *pcLeft;
    *pcLeft++ = *pcString;
    *pcString-- = cX;
  }

  return (pcStart);
}
#endif

//
// Conversion routines numeric type to string
//
#ifndef xtoa
static void xtoa (DWORD dwNumber, char *pcBuffer, UINT uRadix, int iIsNegative)
{
  UINT uDigitValue;        //a_Digito, valuo, weuso
  char *pcX = pcBuffer,    //a_Temp pointer to the string so we can traverse it
       *pcFirstDigit,      //a_Pointer to the first digit
        cTemp;

  //a_Negative, we need a minus '-' and negate the number
  if (iIsNegative)
  {                
    *pcX++ = '-';
    dwNumber = (DWORD)(-(long)dwNumber);
  }

  pcFirstDigit = pcX;           //a_Gotta know where the first digit is

  do
  {
    uDigitValue = (unsigned) (dwNumber % uRadix);   //a_Value of the digit in light of radix
    dwNumber /= uRadix;                             //a_Is next, svimvear!

    //a_Convert to ASCII
    if (uDigitValue > 9)
      *pcX++ = (char)(uDigitValue - 10 + 'a');      //a_We have an alpha
    else
      *pcX++ = (char)(uDigitValue + '0');           //a_We have a digit

  } while (dwNumber > 0);

  //a_Reverse to follow
  *pcX-- = '\0';            //a_NULL terminate since pcX points to last digit in our string

  do
  {
    cTemp = *pcX;
    *pcX = *pcFirstDigit;
    *pcFirstDigit = cTemp;       //a_Prestochangodigito
    --pcX;
    ++pcFirstDigit;             //a_More, more, more...

  } while (pcFirstDigit < pcX); //a_Until we get to the middle, then stop...
}
#endif

// Actual functions just call conversion helper with neg flag set correctly, and return pointer to buffer. */
#ifndef _itoa
char *_itoa (int iNumber, char *pcBuffer, UINT uRadix)
{
  if (uRadix == 10 && iNumber < 0)
    xtoa((DWORD)iNumber, pcBuffer, uRadix, 1);
  else
    xtoa((DWORD)(UINT)iNumber, pcBuffer, uRadix, 0);

  return pcBuffer;
}
#endif

#ifndef _ltoa
char *_ltoa (long lNumber, char *pcBuffer, UINT uRadix)
{
  xtoa((DWORD)lNumber, pcBuffer, uRadix, (uRadix == 10 && lNumber < 0));
  return pcBuffer;
}
#endif

#ifndef _ultoa
char *_ultoa (DWORD dwNumber, char *pcBuffer, UINT uRadix)
{
  xtoa(dwNumber, pcBuffer, uRadix, 0);
  return pcBuffer;
}
#endif


