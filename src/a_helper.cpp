
#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

#include "freeCGI.h"

void AHTMLHelper::formatBinaryData(const char *pccName, AHTML &html, char *pData, DWORD dwLength, WORD wColumns)
{
  if (!pData)
  {
    html << pccName << " = \"(null)\"" << endl;
    return;
  }
  
  if (dwLength == 0)
    dwLength = strlen(pData);
  
  html.htmlStartTag("table", "border=\"1\"");
  
  //a_Length display
  html.htmlStartTag("tr");
  html.htmlStartTag("th", "colspan=\"2\"");
    html << pccName << "  Length: " << dwLength << endl;
  html.htmlEndTag("th");
  html.htmlEndTag("tr");
  
  if (dwLength > 0)
  {
    //a_Column names
    html.htmlStartTag("tr");
    html.htmlStartTag("th");
      html << "Data" << endl;
    html.htmlEndTag("th");
    html.htmlStartTag("th");
      html << "Hex Bytes" << endl;
    html.htmlEndTag("th");
    html.htmlEndTag("tr");

    //a_Display the data
    html.htmlStartTag("td", "bgcolor=\"#000020\"");
    html.htmlStartTag("pre");
    dumpBinaryDataAsPrintable(html, (char *)pData, dwLength, wColumns);
    html.htmlEndTag("pre");
    html.htmlEndTag("td");

    //a_Display hex
    html.htmlStartTag("td", "bgcolor=\"#305010\"");
    html.htmlStartTag("pre");
    dumpBinaryDataAsHex(html, pData, dwLength, wColumns);
    html.htmlEndTag("pre");
    html.htmlEndTag("td");
    html.htmlEndTag("tr");
  }

  html.htmlEndTag("table");
}

void AHTMLHelper::dumpBinaryDataAsHex(AHTML &html, char *pData, DWORD dwLength, WORD wColumns)
{
  char sHex[32];   //a_Hex columns is 'XX '
  int iRow = 0;
  DWORD dwPos = 0, dwRemain = dwLength;
  while (dwRemain > 0)
  {
    int iMax = (dwRemain > wColumns ? wColumns : dwRemain);
    for (int i=0; i<iMax; ++i)
    {
      char c = pData[iRow * wColumns + i];
      
      itoa(c, sHex, 16);
      if (strlen(sHex) == 1)
      {
        sHex[2] = '\x0';
        sHex[1] = sHex[0];
        sHex[0] = '0';
      }

      if (i < wColumns-1)
      {
        sHex[2] = ' ';
        sHex[3] = '\x0';
      }
      else
        sHex[2] = '\x0';
      
      html << sHex;
    }
    ++iRow;
    
    if (dwRemain > wColumns)
      dwRemain -= wColumns;
    else
     dwRemain = 0;
    
    html << endl;
  }
}

void AHTMLHelper::dumpBinaryDataAsPrintable(AHTML &html, char *pData, DWORD dwLength, WORD wColumns)
{
  int iRow = 0;
  DWORD dwPos = 0, dwRemain = dwLength;
  int iDigits = 0;

  while (dwRemain > 0)
  {
    ++iDigits;
    dwRemain >>= 4;
  }
  dwRemain = dwLength;

  char *pOffset = new char [iDigits + 1];
  while (dwRemain > 0)
  {
    int iMax = (dwRemain > wColumns ? wColumns : dwRemain);
    
    //a_Calculate offset
    itoa(iRow * wColumns, pOffset, 16);
    if (strlen(pOffset) < iDigits)
    {
      strrev(pOffset);
      while (strlen(pOffset) < iDigits)
        strcat(pOffset, "0");
      strrev(pOffset);
    }

    //a_Display offset
    html.htmlStartTag("b");
    html << pOffset << ": ";
    html.htmlEndTag("b");

    //a_Display this row
    for (int i=0; i<iMax; ++i)
    {
      char c = pData[iRow * wColumns + i];
      
      if (isprint(c))
      {
        switch(c)
        {
          case '<': html << "&lt;"; break;
          case '>': html << "&gt;"; break;
          case ' ': html << "&nbsp;"; break;
          default: html << c;
        }
      }
      else
        html << ".";
    }
    ++iRow;
    
    if (dwRemain > wColumns)
      dwRemain -= wColumns;
    else
     dwRemain = 0;
    
    html << endl;
  }
  delete []pOffset;
}
