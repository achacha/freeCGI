#ifndef __A_HTML_H__
#define __A_HTML_H__

#ifdef WIN32
#pragma message( "Including " __FILE__ )
#endif

#include "a_predef.h"
#include "a_list.h"

///////////////////////////////////////////////////////////////////////////////////
// AHTML class
///////////////////////////////////////////////////////////////////////////////////
class AHTML : public AStreamOutput
{
  public:
    AHTML(ostream *posOut = NULL) : AStreamOutput(posOut) {}
    virtual ~AHTML() {}
    
    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

    //a_NOTE: \n added to make output readable to the user and is not needed for HTML
    //a_HERE IS A BASIC HTML DOCUMENT (Note: Must send the MIME directive first from a CGI)
    //a_<HTML>
    //a_<HEAD><TITLE>My Title</TITLE></HEAD>
    //a_<BODY>
    //a_</BODY>
    //a_</HTML>

    //a_HTML start & end
    void htmlStartHTML(void)      { outStringCRN("<html>"); };
    void htmlEndHTML(void)        { outStringCRN("</html>"); };
    
    //a_HEAD section and TITLE; htmlDoHEAD() is all that is really needed at this time
    void htmlStartHEAD(void)      { outStringCRN("<head>"); };
    void htmlEndHEAD(void)        { outStringCRN("</head>"); };
    void htmlDoHEAD(const char *pccTitle, const char *pccExtra = NULL);

    void htmlStartTITLE(void)     { outStringCRN("<title>"); };
    void htmlEndTITLE(void)       { outStringCRN("</title>"); };
    void htmlDoTITLE(const char *pccTitle);

    //a_META directive
    void htmlDoMETA(const char *pccName, const char *pccContent);

    //a_BODY directives
    void htmlStartBODY(COLORREF crBack = 0x00808080, COLORREF crText = 0x00FFFFFF,
                       COLORREF crLink = 0x0000DDDD, COLORREF crALink = 0x0000FFFF,
                       COLORREF crVLink = 0x00000000, const char *pccBack = NULL,
                       const char *pccExtra = NULL);
    void htmlStartBODY(AElementPairList &eplBody);
    void htmlEndBODY(void)        { outStringCRN("</body>"); }

    //a_Format: <pccTag pccExtra/>
    void htmlTag(const char *pccTag, const char *pccExtra = NULL);

    //a_Format: <pccTag pccExtra> {pccText} </pccTag>\n and similar
    void htmlStartTag(const char *pccTag, const char *pccExtra = NULL);
    void htmlStartTag(const char *pccTag, AElementPairList &eplTag);
    void htmlEndTag(const char *pccTag);
    void htmlDoTag(const char *pccTag, const char *pccText);
    void htmlDoTagEx(const char *pccTag, AElementPairList &eplTag, const char *pccText);
    void htmlDoTagEx(const char *pccTag, const char *pccExtra, const char *pccText);

    //a_Scripting support
    void htmlStartSCRIPT(const char *pccLanguage = NULL);
    void htmlEndSCRIPT();

    //a_Space skipping using &nbsp; control character
    void htmlDoSpace(int iN = 0x1);
    
    //a_The comment <!-- ... -->
    void htmlDoComment(const char *pcc1, const char *pcc2 = NULL, const char *pcc3 = NULL);

    //a_Helper functions
    void htmlDateTime(void)
    {
      time_t ttNow = time(NULL);
      outString(ctime(&ttNow));
    }

    //a_Generic simple MIME output (type/subtype  and content length for binary types such as GIF or JPEG)
    void mimeOut(const char *pccType, const char *pccSubType, int iContentLength = -1, ACookie *paCookie = NULL, int iCookieCount = 1);
    //a_MIME directive for HTML output with HTTP Cookie support
    void mimeHTML(ACookie *paCookie = NULL, int iCookieCount = 1);
    //a_Only callable by AXBitmap (so far) when outputing x-xbitmap
    void mimeXBitmap(void)
    { 
      mimeOut("image", "x-xbitmap");
    }
};

//a_Global redirection operators (to avoid declaration matching), uses polymorphism of AElement
inline AStreamOutput &operator <<(AStreamOutput &ahtmlOut, const ABaseElement &abeSource)
{
  abeSource.doOut(&ahtmlOut);
  return ahtmlOut;
}

#endif

