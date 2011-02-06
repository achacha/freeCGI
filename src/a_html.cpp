
#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

#include "freeCGI.h"

#ifdef _DEBUG_DUMP_
void AHTML::dump(void)
{
  cout << endl << "---START: AHTML::dump()---";
  AStreamOutput::dump();                                 //a_Do parent dump as well
  cout << endl << "---END: AHTML::dump()---" << endl;
}
#endif

void AHTML::mimeOut(const char *pccType, const char *pccSubType, int iContentLength, ACookie *paCookie, int iCookieCount)
{
  if (pccType && pccSubType)
  {
    if (pccType && pccSubType)
    {
      *this << "Content-Type: " << pccType << "/" << pccSubType << "\r\n";    
      
      //a_Do cookie if any
      if (paCookie)
      {
        for (int i = 0; i < iCookieCount; i++)
        {
          (paCookie + i)->doOut(this);         //a_Dump it
          outStringN("\r\n");
        }
      }
      
      //a_Do content length if any
      if (iContentLength >= 0x0)
        *this << "Content-Length: " << iContentLength << "\r\n" << flush;
    }
  }
  else
    assert(0x0);   //a_If you got this, then you passed at least one NULL for the MIME type or sub-type

  outStringN("\r\n"); 
}

//a_mime directive with Cookie support
void AHTML::mimeHTML(ACookie *paCookie, int iCookieCount)
{
  outStringN("Content-Type: text/html\r\n");
  if (paCookie)
  {
    for (int i = 0; i < iCookieCount; i++)
    {
      (paCookie + i)->doOut(this);         //a_Dump it
      outStringN("\r\n");
    }
  }
  outStringN("\r\n");
}

void AHTML::htmlDoHEAD(const char *pccTitle, const char *pccExtra)
{
  outStringN("<head><title>");
  outString(pccTitle);
  if (pccExtra)
    outString(pccExtra);               //a_Convenience append string
  outStringCRN("</title></head>");
}

void AHTML::htmlDoTITLE(const char *pccTitle)
{
  outStringN("<title>");
  outString(pccTitle);
  outStringCRN("</title>");
}

//a_HTML output format as follows:
//a_<BODY BACGROUNG=(pccBack) BGCOLOR="crBack" TEXT="crText" LINK="crLink" ALINK="crALink" VLINK="crVLink">
//a_ie. VistitedLINK(if visitied) and ActiveLINK(while clicked)
void AHTML::htmlStartBODY(COLORREF crBack, COLORREF crText, COLORREF crLink, COLORREF crALink, COLORREF crVLink, const char *pccBack, const char *pccExtra)
{
  outStringN("<body");
  if (pccBack)
  {
    outStringN(" background=");
    outStringQ(pccBack);
  }

  //a_Now do the colors (temp local buffer is to insure re-entrant code for the function called)
  char sColor[8];
  outStringN(" bgcolor=");
  outStringQ(g_aCrypto.convertoCOLORREFtoChar(crBack, sColor));
  outStringN(" text=");
  outStringQ(g_aCrypto.convertoCOLORREFtoChar(crText, sColor));
  outStringN(" link=");
  outStringQ(g_aCrypto.convertoCOLORREFtoChar(crLink, sColor));
  outStringN(" alink=");
  outStringQ(g_aCrypto.convertoCOLORREFtoChar(crALink, sColor));
  outStringN(" vlink=");
  outStringQ(g_aCrypto.convertoCOLORREFtoChar(crVLink, sColor));
  if (pccExtra)
  {
    outStringN(" ");
    outString(pccExtra);
  }
  outStringCRN(">");
}

void AHTML::htmlStartBODY(AElementPairList &eplBody)
{
  outStringN("<body ");
  *this << " " << eplBody;
  outStringCRN(">");
}

void AHTML::htmlStartTag(const char *pccTag, const char *pccExtra)
{ 
  if (pccTag)
  {
    outStringN("<");
    outString(pccTag);
    if (pccExtra)
    {
      outStringN(" ");
      outString(pccExtra);
    }
    outStringN(">");
  }
}

void AHTML::htmlStartTag(const char *pccTag, AElementPairList &eplTag)
{
  if (pccTag)
  {
    outStringN("<");
    outString(pccTag);
    *this << " " << eplTag;
    outStringCRN(">");
  }
}

void AHTML::htmlEndTag(const char *pccTag)
{ 
  if (pccTag)
  {
    outStringN("</");
    outString(pccTag);
    outStringN(">");
  }
}

void AHTML::htmlTag(const char *pccTag, const char *pccExtra)
{
  if (pccTag)
  {
    outStringN("<");
    outString(pccTag);
    if (pccExtra)
    {
      outStringN(" ");
      outString(pccExtra);
    }
    outStringN("/>");
  }
}

void AHTML::htmlDoTag(const char *pccTag, const char *pccText)
{ 
  htmlDoTagEx(pccTag, NULL, pccText);
}

void AHTML::htmlDoTagEx(const char *pccTag, const char *pccExtra, const char *pccText)
{ 
  if (pccText)
  {
    htmlStartTag(pccTag, pccExtra);
    outString(pccText);
    htmlEndTag(pccTag);
  }
}

void AHTML::htmlDoTagEx(const char *pccTag, AElementPairList &eplTag, const char *pccText)
{ 
  if (pccText)
  {
    htmlStartTag(pccTag, eplTag);
    outString(pccText);
    htmlEndTag(pccTag);
  }
}

void AHTML::htmlDoSpace(int iN)
{
  while (iN-- > 0x0)
  {
    //a_13 spaces per line
    if ((iN % 13) == 0x0) outStringCRN("&nbsp;");
    else                  outStringN("&nbsp;");
  }
}

void AHTML::htmlDoComment(const char *pcc1, const char *pcc2, const char *pcc3)
{
  //a_Embed a bunch of text inside a comment (Shouldn't contain "-->" in the pccN's...)
  outStringN("<!--");
    if (pcc1) outString(pcc1);
    if (pcc2) outString(pcc2);
    if (pcc3) outString(pcc3);
  outStringCRN("-->");
}


void AHTML::htmlStartSCRIPT(const char *pccLanguage)
{
  outStringN("<script language=");
  if (pccLanguage)
    outStringQ(pccLanguage);   //a_A script language
  else
    outStringQ("JavaScript");  //a_Default script language

  outStringCRN("><!-- Hide from older browsers");
}

void AHTML::htmlEndSCRIPT()
{
  outStringCRN("//--></script>");
}

void AHTML::htmlDoMETA(const char *pccName, const char *pccContent)
{
  if (pccName && pccContent)
  {
    outStringN("<meta name=");
    outStringQ(pccName);
    outStringN(" content=");
    outStringQ(pccContent);
    outStringCRN(">");
  }
  else
  {
    #ifdef _DEBUG_FULL
      htmlDoComment("META directive contained a NULL pointer");
    #endif
  }
}
