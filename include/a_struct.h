//////////////////////////////////////////////////////////
// Structure definitions
//////////////////////////////////////////////////////////

#ifndef __A_STRUCT_H__
#define __A_STRUCT_H__

#ifdef WIN32
#pragma message( "Including " __FILE__ )
#endif

#include "a_predef.h"

//a_CRC Model structure
//a_Initialized to the CRC constants upon call to one of the CRC functions
//a_You can pass it a pointer to your own struct. I used the values in the article. 
typedef struct  tagCRCMODEL
{
  int   iWidth;    //Width in bits [8,32]
  DWORD dwPoly;    //The algorithm's polynomial.
  DWORD dwInit;    //Initial register value.
  BYTE  bRefIn;    //Reflect input bytes?
  BYTE  bRefOut;    //Reflect output CRC?
  DWORD dwXOROut;  //XOR this to output CRC.

  DWORD dwReg;     //Context during execution.

  //a_Initialization
  void Init(void) { dwReg = dwInit; }

} CRCMODEL, *PCRCMODEL;


//a_Parameters for outputting characters
typedef struct tagTEXTOUT
{
  const char *pccText;          //a_Output text (will be mapped thorogh the character set and then blitted)
  int         iTextLength;      //a_Length of the output text
  int         iPX, iPY;         //a_Starting offset for the line
  int         iSpacing;         //a_Intercharacter spacing
  int         iBaseline;        //a_How far from top of line is the basline (NOTE: too low may clip against iLineHeight)
  int         iLineHeight;      //a_Height of the line
  int         iMode;            //a_Method of blitting
  int         iAlign;           //a_Align (left or right)

  tagTEXTOUT(const char *pccX = NULL, int iLength = -0x1)
  {
    if (pccX)
    {
      //a_Basic string assign
      pccText = pccX;
      if (iLength > 0x0) iTextLength = iLength;
      else iTextLength = strlen(pccX);
      
    }
    else
    {
      pccText = NULL;
      iTextLength = 0x0;                       //a_No default text
    }

    iMode = BLIT_COPY;                         //a_Copy by default
    iPX = iPY = 0x0;                           //a_Assume start at 0,0
    iSpacing = 0x1;                            //a_Default character spacing
    iBaseline = iLineHeight = -0x1;            //a_Not set
    iAlign = HALIGN_LEFT;                      //a_See predef.h (constant used in other places)
  }
} TEXTOUT;

#endif
