#ifndef __A_HTMLHELPER_H__
#define __A_HTMLHELPER_H__

#ifdef WIN32
#pragma message( "Including " __FILE__ )
#endif

#include "a_predef.h"
#include "a_html.h"

///////////////////////////////////////////////////////////////////////////////////
// AHTMLHelper class
///////////////////////////////////////////////////////////////////////////////////
class AHTMLHelper 
{
  public:
    //a_Given data and length will format the binary data in a somewhat viewable format
    //a_One column will contain printable characters, other will contain hex code
    //a_iColumns is data bytes per row
    //a_Output will be in a <table>, pccName is the title of the table
    static void formatBinaryData(const char *pccName, AHTML &html, char *pData, DWORD dwLength = 0, WORD wColumns = 32);
    
    //a_Used by the format function above, this will not format the data, but rather just dump it to output
    //a_Display hex code in format "XX XX XX XX ..." one entry per column
    static void dumpBinaryDataAsHex(AHTML &html, char *pData, DWORD dwLength, WORD wColumns = 32);

    //a_Used by the format function above, this will not format the data, but rather just dump it to output
    //a_Displays if printable as "OFFSET: Some data here ...", offset in hex
    static void dumpBinaryDataAsPrintable(AHTML &html, char *pData, DWORD dwLength, WORD wColumns = 32);
};
#endif

