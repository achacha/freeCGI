#ifndef __A_CGI_H__
#define __A_CGI_H__

#ifdef WIN32
#pragma message( "Including " __FILE__ )
#endif

#include "a_predef.h"
#include "a_list.h"

///////////////////////////////////////////////////////////////////////////////////
// ACGI class, inherits HTML features from AHTML and Form processing from AFormList 
///////////////////////////////////////////////////////////////////////////////////
class ACGI : virtual public AHTML, virtual public AFormList
{
  public:
    //a_Standard constructor that allows redirection of the output
    ACGI(ostream *posOut = NULL) : AHTML(posOut) {}
    //a_Used when generating inlines, given MIME type/subtype and filename to output
    ACGI(const char *pccMIMEType, const char *pccMIMESubType, const char *pccFilePath)
    {
      cgiOutputBinary(pccMIMEType, pccMIMESubType, pccFilePath);
    }
    virtual ~ACGI() {}

    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif
    
    void cgiEnvironmentDump(int iFullDump = 0x0);

    //a_Internet related
    DWORD cgiGetIP(const char *pccIP = NULL);

    //a_FORM elements (Action==NULL mean use this script name, self posting)
    //a_See a_predef for FORMINPUT_xxx types usable by other sources
    void cgiStartFORM(const char *pccAction = NULL, const char *pccMethod = NULL);      //a_Simple FORM parameters
    void cgiStartFORM(AElementPairList &eplItems) { htmlStartTag("FORM", eplItems); }   //a_Extended FORM parameters (using AHTML wrapper)
    void cgiDoFORMInput(int iType, AElementPairList &eplItems, const char *pccContent = NULL, const char *pccExtra = NULL);
    void cgiDoFORMInput(int iType, const char *pccName, const char *pccValue = NULL, const char *pccContent = NULL, const char *pccExtra = NULL);
    void cgiEndFORM(void) { outStringCRN("</FORM>"); }                                  //a_End the FORM

    //a_Form related: POST method implies both POST and GET, whereas GET is only a GET
    int cgiGetFormItems(istream *pisInput = NULL);            //a_Defaults to cin/QUERY_STRING if no istream is specified (usually not specified!)
    int cgiGetFormItems(const char *pccInput);                //a_String based, input known (good for debugging)
	  int cgiGetQueryStringItems(const char *pccInput = NULL);  //a_Only gets QueryString assuming it is a GET (the other 2 incluse a call to this)

    //a_Checking functions
    int cgiIsGET(void) { return (strstr(cgiGetRequestMethod(), "GET") == NULL ? 0x0 : 0x1); }
    int cgiIsPOST(void) { return (strstr(cgiGetRequestMethod(), "POST") == NULL ? 0x0 : 0x1); }

    //a_Valid input checking
    int cgiIsValidEMail(const char *pccTest);
    int cgiIsWithoutMetaChar(const char *pccTest, int iStrict = 0x0);
    int cgiIsValidHTMLTag(const char *pccTest);
    int cgiIsValidURLProtocol(const char *pccTest);
    int cgiIsValidURL(const char *pccTest);

    //a_HTML specific checks and corrections
    int  cgiIsNotValidHTMLLine(const char *pccTest);           //a_Checks if it is a valid HTML line was entered in a form
    char *cgiValidateHTMLLine(char *pcLine);                   //a_Validates the HTML line by putting comment where invalid statement is

    //a_Outputing a URL after encoding
    void cgiEncodeAndOutputURL(const char *pccSource, int iLength = -0x1);  //a_Encodes and outputs to the default stream

    //a_Binary output (must be the only function called!)
    //a_This is usable for GIF and JPEG output types image/gif and image/jpeg MIMEs
    int cgiOutputBinary(const char *pccMIMEType, const char *pccMIMESubType, const char *pccFilePath);

    //a_Get the desired environment variable, inlined for speed!
    const char *cgiGetSafeEnv(const char *pEnvName)
    {
      const char *pccValue = getenv(pEnvName);      
      return (pccValue ? pccValue : NULL_STRING);
    } 
  
    //a_HTTP specific (from RFC 2616 for HTTP/1.1)
    const char *cgiGetHTTPAccept(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_ACCEPT");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPAcceptEncoding(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_ACCEPT_ENCODING");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPAcceptCharset(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_ACCEPT_CHARSET");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPAcceptLanguage(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_ACCEPT_LANGUAGE");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPAuthorization(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_AUTHORIZATION");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPCookie(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_COOKIE");  //a_Used with ACookie
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPExpect(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_EXPECT");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPFrom(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_FROM");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPHost(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_HOST");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPIfMatch(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_IF_MATCH");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPIfModifiedSince(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_IF_MODIFIED_SINCE");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPIfNoneMatch(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_IF_NONE_MATCH");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPIfRange(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_IF_RANGE");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPIfUnmodifiedSince(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_IF_UNMODIFIED_SINCE");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPMaxForwards(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_MAX_FORWARDS");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPProxyAuthorization(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_PROXY_AUTHORIZATION");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPReferer(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_REFERER");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPUserAgent(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_USER_AGENT");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPPragma(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_PRAGMA");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPRange(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_RANGE");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHTTPTE(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HTTP_TE");
      if (iOut) outStringN(pccX);
      return pccX;
    }

    //a_Non-HTTP related (server specific often)
    const char *cgiGetAnnotationServer(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("ANNOTATION_SERVER");
      if (iOut) outStringN(pccX);
      return pccX;
    }
  
    //a_Path info
    const char *cgiGetPath(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("PATH");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetPathInfo(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("PATH_INFO");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetPathTranslated(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("PATH_TRANSLATED");
      if (iOut) outStringN(pccX);
      return pccX;
    }
  
    //a_Server/Gateway variables
    const char *cgiGetServerSoftware(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("SERVER_SOFTWARE");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetServerName(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("SERVER_NAME");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetServerPort(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("SERVER_PORT");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetServerProtocol(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("SERVER_PROTOCOL");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetGatewayInterface(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("GATEWAY_INTERFACE");
      if (iOut) outStringN(pccX);
      return pccX;
    }

    //a_Remote specific variable
    const char *cgiGetRemoteHost(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("REMOTE_HOST");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetRemoteAddress(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("REMOTE_ADDR");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetRemoteIdent(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("REMOTE_IDENT");
      if (iOut) outStringN(pccX);
      return pccX;
    }

    //a_Request variables
    const char *cgiGetContentType(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("CONTENT_TYPE");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetContentLength(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("CONTENT_LENGTH");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetRequestMethod(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("REQUEST_METHOD");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetQueryString(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("QUERY_STRING");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetScriptName(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("SCRIPT_NAME");
      if (iOut) outStringN(pccX);
      return pccX;
    }

    //a_System specific variables
    const char *cgiGetSHLVL(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("SHLVL");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetPWD(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("PWD");
      if (iOut) outStringN(pccX);
      return pccX;
    }          
    const char *cgiGetLogName(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("LOGNAME");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetUser(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("USER");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHost(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HOST");
      if (iOut) outStringN(pccX);
      return pccX;
    }
    const char *cgiGetHostType(int iOut = 0x0)
    { 
      const char *pccX = cgiGetSafeEnv("HOSTTYPE");
      if (iOut) outStringN(pccX);
      return pccX;
    }

  protected:
    //a_FORM related output
    int _cgiDoFormItems(istream *pisInput = NULL);         //a_The main function for form item retrieval
    void _cgiDoInputType(int iType);                       //a_Output FORM items TYPE
};

#endif

