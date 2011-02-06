#ifndef __A_GLOBAL_H__
#define __A_GLOBAL_H__

#ifdef WIN32
#pragma message( "Including " __FILE__ )
#endif

#include "a_predef.h"

//a_Buffer manipulation
char *aStrDup(const char *pccNew, int iLength = -0x1, int iNullTerminate = 0x1);
char *aStrCat(const char *pcc1, const char *pcc2);
char *aStrCat3(const char *pcc1, const char *pcc2, const char *pcc3);
BYTE *aMemDup(const BYTE *pbNew, int iLength);
BYTE *aMemCat(const BYTE *pb1, int iL1, const BYTE *pb2, int iL2);

//a_ANSI extensions
#ifndef _strrev
char * _strrev (char *pcString);
#endif

#ifndef strrev
#define strrev _strrev
#endif

#ifndef xtoa
static void xtoa (DWORD dwNumber, char *pcBuffer, UINT uRadix, int iIsNegative);
#endif
#ifndef _itoa
char *_itoa (int iNumber, char *pcBuffer, UINT uRadix);
#ifndef itoa
#define itoa _itoa
#endif
#endif

#ifndef _ultoa
char *_ultoa (DWORD dwNumber, char *pcBuffer, UINT uRadix);
#endif

#endif
