//a_AElementPairList and items based on it (HTML/CGI items mostly)
#ifndef __A_ELIST_H__
#define __A_ELIST_H__

#ifdef WIN32
#pragma message( "Including " __FILE__ )
#endif

#include "a_predef.h"
#include "a_list.h"

//////////////////////////////////////////////////////////////////////
// AElementPairListList of NAME=VALUE pairs in HTML based documents, URLs, et al.
// Can also be used as a parameter list for a URL
//////////////////////////////////////////////////////////////////////
class AElementPairList : public APairList
{
  public:
    AElementPairList(AElementPairList *pelSource = NULL);    //a_Default and Copy constructor!
    virtual ~AElementPairList();
    
    //a_Class based output
    virtual void doOut(AStreamOutput *pasOut) const   //a_NAME1="VALUE1" NAME2="VALUE2" Quotes, no URL encode
      { APairList::doPairs(pasOut, ' ', INT_MAX, 0x1, 0x0); }
    void doURLParams(AStreamOutput *pasOut) const     //a_N1=V1&N2=V2&N3=V3... No quotes, URL encode
      { APairList::doPairs(pasOut, '&', INT_MAX, 0x0, 0x1); }

    //a_Equals operator (for tag copying)
    AElementPairList &operator =(const AElementPairList &aeSource)   
      { _bCopy(aeSource); return *this; }

    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

    //a_To add an ecoded member, just use eAddEncoded
    //a_To decode, call eDecodeAndGetUserData with the key name to retieve data and length
    void        elAddEncoded(const char *pccKName, const BYTE *pcbUserData, int iLength, int iMethod = AConverto::eat4Bit);
    const BYTE *elDecodeAndGetUserData(const char *pccKName, int &iLength, int iMethod = AConverto::eat4Bit);

  protected:
    //a_Copy function (usable to children)
    void _bCopy(const AElementPairList &aeSource);
    
    //a_Used for encoding/decoding element members (no need to copy, used as persistent return values)
    BYTE *m_pbUserData;           //a_User data
    WORD  m_wUserDataLength;      //a_User data length
};

///////////////////////////////////////////////////////////////////////////
// AElement of HTML based documents: <{TAG} ...NAME=VALUE parameters...>
///////////////////////////////////////////////////////////////////////////
class AElement : public AElementPairList
{
  public:
    AElement(AElement *paeSource = NULL);    //a_Default and Copy constructor!
    ~AElement();
    
    //a_Class based output
    virtual void doOut(AStreamOutput *pasOut) const;  //a_One per line N=V
    virtual void doEnd(AStreamOutput *pasOut) const;  //a_Ends this element </{m_pcTag>

    //a_Equals operator (for tag copying)
    AElement &operator =(const AElement &aeSource)   
      { _bCopy(aeSource); return *this; }
  
    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

    //a_HTML Element name
    void        eSetTag(const char *pccX);
    const char *eGetTag(void) const  { return m_pcTag; }

  protected:
    //a_Copy function (usable to children)
    void _bCopy(const AElement &aeSource);

    //a_Tag of this HTML element, if not present then it is just a list of parameter, '<' and '>' are not outputted
    char *m_pcTag;
};


//////////////////////////////////////////////////////////////////////
// ACookie element from HTTP-Cookie support
// SEE: docs/cookie_spec.html for more info
//////////////////////////////////////////////////////////////////////
class ACookie : public AElementPairList
{
  public:
    ACookie() {};
    virtual ~ACookie() {};
   
    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

    //a_Ouput function for HTTP_COOKIE, used by AHTML object in mimeHTML() method.
    virtual void doOut(AStreamOutput *pasOut) const;
};

//////////////////////////////////////////////////////////////////////
// AURL - Universal Resource Locator - parser/generator
//////////////////////////////////////////////////////////////////////
//a_This is the URL format that I used here
//URL_PROTOCOL://URL_NAME:URL_PASSWORD@URL_HOST:URL_PORT/URL_PATH{?#}URL_PARAM = {?QUERY || #REF}
//                                                                  
//a_Parsing method:
//1. Reverse parse to find '?' and to the right is URL_QUERY or '#' for URL_REF, remove
//2. Reverse find the first '/', this is URL_FILE.URL_EXTENSION, extract UEXT and remove
//3. Reverse find "//" or start, that is the URL_HOST:URL_PORT/URL_PATH, split and remove
//4. Get the URL_PROTOCOL or use "http:" as a default
class AURL : public AElementPairList
{
  public:
    AURL(const char *pccURL = NULL)
    { 
      if (pccURL)
        _urlParseAndSet(pccURL);
    }
    virtual ~AURL() {};

    //a_Class based output
    virtual void doOut(AStreamOutput *pasOut) const;

    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

    //a_Setting URL
    void urlSet(const char *pccURL = NULL);

    //a_Statistics
    int urlIsValidURL(void) const;

    //a_Protocl validity checking (takes the name of protocol without the ':', ie. "http")
    int urlIsValidProtocol(const char *pccTest = NULL) const;

  protected:
    //a_URL parsing
    void _urlParseAndSet(const char *pccURL);
};

#endif


