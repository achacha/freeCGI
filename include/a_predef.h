//a_predef.h
#ifndef __A_PREDEF_H__
#define __A_PREDEF_H__

#ifdef WIN32
#pragma message( "Including " __FILE__ )
#endif

#if _MSC_VER > 1000 || __SUNPRO_CC >= 1280 || __GNUC__ >= 2
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;
#else
#include <iostream.h>
#include <strstrea.h>
#include <fstream.h>
#endif

#if _MSC_VER > 1000
  #pragma once

  //a_Level 4 warning to ignore
  #pragma warning(disable : 4244)  //a_'argument' : conversion from '...' to '...', possible loss of data
  #pragma warning(disable : 4127)  //a_conditional expression is constant
#endif

//a_These are defines to be used from the Standard C/C++ Runtime library
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <float.h>
#include <math.h>

//a_The NULL string, useful for runtime functions that can't handle NULL pointers
//a_iostream's are an example of this, as are string/memory manipulation routines
#define NULL_STRING "(null)"
#define SAFESTRING(pcc) (pcc ? pcc : NULL_STRING)

//a_min/max functions
//a_Min/Max support for some UNIX platforms :)
#ifndef __min
#define __min(A,B) ((A) > (B) ? (B) : (A))
#endif
#ifndef __max
#define __max(A,B) ((A) > (B) ? (A) : (B))
#endif

//a_Unsigned fixed types, 1 byte, 2 bytes, and 4 bytes, and machine specific unsigned int
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned int UINT;

//a_Access MACROs for the unsigned types
#if !defined(__AFX_H__) && !defined(_WINDOWS_)       //a_Windows.h and MFC already defines these
#define MAKEWORD(hi, lo)    ((WORD)(((BYTE)(lo)) | ((WORD)((BYTE)(hi))) << 0x8))
#define HIWORD(dw)          ((WORD)(((DWORD)(dw) >> 0x10) & 0xFFFF))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 0x8) & 0xFF))
#endif

#define MAKEDWORD(hi, lo)   ((DWORD)(((WORD)(lo)) | ((DWORD)((WORD)(hi))) << 0xF))
#define LOWORD(dw)          ((WORD)(dw))
#define LOBYTE(w)           ((BYTE)(w))

#define BYTE0(dw)           ((BYTE)(dw))
#define BYTE1(dw)           ((BYTE)(((WORD)(dw)  >> 0x8)  & 0xFF))
#define BYTE2(dw)           ((BYTE)(((DWORD)(dw) >> 0x10)  & 0xFF))
#define BYTE3(dw)           ((BYTE)(((DWORD)(dw) >> 0x18) & 0xFF))

#define BITMASK(n)          (0x1L << (n))
#define WIDTHMASK(n)        ((((0x1L << ((n) - 0x1)) - 0x1L) << 0x1) | 0x1L)

//a_Ceiling MACRO in multiple on N
#define CEILN(num, N)       (((num + N - 1) / N) * N)
#define FLOORN(num, N)      ((num / N) * N)

//a_BLITting constants
#define BLIT_COPY 0x0
#define BLIT_AND  0x1
#define BLIT_OR   0x2
#define BLIT_XOR  0x3

//a_COLORREF, used for color definitions
typedef unsigned long COLORREF;          //a_0x00bbggrr

//a_RGB
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define GetRValue(rgb)      ((BYTE)(rgb))
#define GetGValue(rgb)      ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      ((BYTE)((rgb)>>16))
//a_Value(brightness) of the COLORREF
#define GetVValue(rgb)      ((BYTE)(max(((BYTE)(rgb)),max(((BYTE)(((WORD)(rgb)) >> 8)),((BYTE)((rgb)>>16))))))
//a_Anti-Value(dullness) of the COLORREF
#define GetLValue(rgb)      ((BYTE)(min(((BYTE)(rgb)),min(((BYTE)(((WORD)(rgb)) >> 8)),((BYTE)((rgb)>>16))))))

//a_Quick COLORREF values D(ark) M(edium) L(ight)
#define RGB_BLACK    0x00000000
#define RGB_WHITE    0x00FEFEFE

#define RGB_DGRAY    0x00666666
#define RGB_MGRAY    0x008C8C8C
#define RGB_LGRAY    0x00D9D9D9

#define RGB_DRED     0x00000066
#define RGB_MRED     0x0000008C
#define RGB_LRED     0x000000D9

#define RGB_DGREEN   0x00006600
#define RGB_MGREEN   0x00008C00
#define RGB_LGREEN   0x0000D900

#define RGB_DBLUE    0x00660000
#define RGB_MBLUE    0x008C0000
#define RGB_LBLUE    0x00D90000

#define RGB_DCYAN    0x00666600
#define RGB_MCYAN    0x008C8C00
#define RGB_LCYAN    0x00D9D900

#define RGB_DMAGENTA 0x00660066
#define RGB_MMAGENTA 0x008C008C
#define RGB_LMAGENTA 0x00D900D9

#define RGB_DYELLOW  0x00006666
#define RGB_MYELLOW  0x00008C8C
#define RGB_LYELLOW  0x0000D9D9

#ifdef _MSC_VER
  //a_Disable Microsoft C based warning so they don't annoy me at level 4 output :)
  //a_4244 - conversion from 'int' to 'char', possible loss of data
  //a_4706 - assignment within conditional expression (I like to do that :)
  #pragma warning( disable : 4244 4706 )
#endif

//a_RTTI based defines....
//a_ _CPPRTTI for VC++ and BC++ enables it by default
#if defined(_CPPRTTI) || defined(__BORLANDC__) || defined(_RTTI_)
#define CAST(vtype, vclass)         (dynamic_cast<vtype>(vclass))          //a_Dynamic RTTI casting
#define ASSERT_CLASS(vtype, vclass) (assert(dynamic_cast<vtype>(vclass)))  //a_If vclass is a reference then bad_cast will be thrown!
#else
#define CAST(vtype, vclass)         ((vtype)vclass)                        //a_Basic C cast
#define ASSERT_CLASS(vtype, vclass) ((void *)NULL)                         //a_Do nothing but cast (may not work correctly for some multiple inheritance cases)...
#endif

//a_Alignment constants
#define HALIGN_LEFT   0x1
#define HALIGN_CENTER 0x2
#define HALIGN_RIGHT  0x4

//a_FORM Input types
#define FORMINPUT_HIDDEN   0x00
#define FORMINPUT_CHECKBOX 0x01
#define FORMINPUT_RADIO    0x02
#define FORMINPUT_TEXT     0x03
#define FORMINPUT_PASSWORD 0x04
#define FORMINPUT_RANGE    0x05
#define FORMINPUT_SUBMIT   0x06
#define FORMINPUT_RESET    0x07
#define FORMINPUT_TEXTAREA 0x08
#define FORMINPUT_IMAGE    0x09
#define FORMINPUT_SCRIBBLE 0x0A
#define FORMINPUT_FILE     0x0B

//a_Maximum buffer for reading TEXTAREA input
//a_Buffer used to process the stream (will contain the Name and Value elements)
//a_<textarea> item may contain a decent amount of Value, so adjust accordingly
#define FL_MAX_INPUT_BUFFER 16384

//a_CRC, checksums, etc.. in AConverto
//a_Provided different methods for added security
#define DP_XOR8      0x0001
#define DP_XOR16     0x0002
#define DP_XOR32     0x0003
#define DP_MOD8SUM   0x0010
#define DP_MOD16SUM  0x0011
#define DP_MOD32SUM  0x0012

//a_Defines used an NAMEs in AElementList
#define URL_PROTOCOL          "UPROT"
#define URL_NAME              "UNAME"
#define URL_HOST              "UHOST"
#define URL_PORT              "UPORT"
#define URL_PATH              "UPATH"
#define URL_PARAM             "UPARM"

//a_Defaults to use
#define URL_PROTOCOL_DEFAULT  "http"
#define URL_PATH_DEFAULT      "/"

//a_This is the URL format that I used here
//URL_PROTOCOL://URL_NAME:URL_PASSWORD@URL_HOST:URL_PORT/URL_PATH{?#}URL_PARAM = {?QUERY || #REF}
//
//a_Parsing method:
//1. Reverse parse to find '?' and to the right is URL_QUERY or '#' for URL_REF, remove
//2. Reverse find the first '/', this is URL_FILE.URL_EXTENSION, extract UEXT and remove
//3. Reverse find "//" or start, that is the URL_HOST:URL_PORT/URL_PATH, split and remove
//4. Get the URL_PROTOCOL or use "http:" as a default

//a_ID of the protocol used (returned by IsValidProtocol member)
#define URL_ID_BAD    0x0
#define URL_ID_HTTP   0x1
#define URL_ID_FTP    0x2
#define URL_ID_FILE   0x3
#define URL_ID_GOPHER 0x4
#define URL_ID_MAILTO 0x5
#define URL_ID_NEWS   0x6
#define URL_ID_TELNET 0x7
#define URL_ID_TN3270 0x8
#define URL_ID_RLOGIN 0x9
#define URL_ID_WAIS   0xA

//a_Output format NAME="VALUE" in APairItem (most common behavior)
#define PI_QUOTED      0x1
//a_Output in APaitItem for {VALUE} will be URL Encoded before outputting
#define PI_URLENCODED  0x2

//a_Used for character sets
#define CSM_DEFAULT_LINEHEIGHT 32     //a_Since the sets are all 32x32, this is default
#define CSM_DEFAULT_BASELINE   13     //a_In a 32x32, it's a good baseline, allowing enough above and below, since few are 31 bits high...

//a_Shopping Cart related
#define ASC_SEPARATOR '.'             //a_Used to separate {CartName}{Quantity}


#endif // __A_PREDEF_H__

