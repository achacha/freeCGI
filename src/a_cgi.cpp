
#include "freeCGI.h"

#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

#define READ_BUFFER_SIZE 1536       //a_Some read buffer size

#ifdef _DEBUG_DUMP_
void ACGI::dump()
{
  cout << endl << "---START: ACGI::dump()---";
  AHTML::dump();
  AFormList::dump();
  cout << endl << "---END: ACGI::dump()---" << endl;
}
#endif

int ACGI::cgiGetQueryStringItems(const char *pccInput)
{
  int iRet = 0x0;
  if (!pccInput)
  {
    //a_Try to get the QueryString
    #ifdef _DEBUG_DUMP_
      cout << "<!--ACGI::cgiGetQueryStringItems(const char *)=" << cgiGetQueryString() << "-->";
    #endif
    if (!strstr(cgiGetQueryString(), NULL_STRING))
    {
      //a_Something is in there
      iRet = flGenerateList(cgiGetQueryString());
    }
  }
  else
  {
    #ifdef _DEBUG_DUMP_
      cout << "<!--ACGI::cgiGetQueryStringItems(const char *)=" << pccInput << "-->";
    #endif
    iRet = flGenerateList(pccInput);
  }

  return iRet;
}

int ACGI::cgiGetFormItems(istream *pisInput)
{
  #ifdef _DEBUG_DUMP_
    cout << "<!--ACGI::cgiGetFormItems(istream *)-->";
  #endif

  return _cgiDoFormItems(pisInput);
}

int ACGI::cgiGetFormItems(const char *pccInput)
{
  #ifdef _DEBUG_DUMP_
    cout << "<!--ACGI::cgiGetFormItems(const char *)-->";
  #endif

  if (pccInput)
    return flGenerateList(pccInput);

  return _cgiDoFormItems();
}

int ACGI::_cgiDoFormItems(istream *pisInput)
{
  if (pisInput)
  {
    //a_Another stream used for inputing
    #ifdef _DEBUG_DUMP_
      cout << "<!--ACGI::_cgiDoFormItems: istream-->";
    #endif
    return flGenerateList(pisInput, -1);
  }
  else
  {
    if (cgiIsGET())
    {
      #ifdef _DEBUG_DUMP_
        cout << "<!--ACGI::_cgiDoFormItems: GET-->";
      #endif
      return flGenerateList(cgiGetQueryString());
    }
    else
    {
      if (cgiIsPOST())
      {
        #ifdef _DEBUG_DUMP_
          cout << "<!--ACGI::_cgiDoFormItems: POST and GET-->";
        #endif
        return flGenerateList(&cin, atoi(cgiGetContentLength())) + flGenerateList(cgiGetQueryString());;
      }
      else
      {
        //a_Not a POST or a GET (hmm... HEAD, PUT, LINK, UNLINK, DELETE and FILE not yet supported (inadequate documentation))
        #ifdef _DEBUG_DUMP_
          cout << "<!--ACGI::_cgiDoFormItems: Neither POST nor GET-->";
        #endif
      }
    }
  }

  return 0x0;
}

//a_Checks for EMail in the form of name@address.dom and has no HTML tags embedded
int ACGI::cgiIsValidEMail(const char *pccTest)
{
  if (pccTest && cgiIsNotValidHTMLLine(pccTest))
  {
    const char *pccX = strchr(pccTest, '@');
    if (!pccX)
      return 0x0;                    //a_Must have name@address
    
    //a_Domain must have at least 1 period if {com, edu, net, org, gov, mil, int, us, etc}
    pccX = strchr(pccX, '.');
    if (!pccX)
      return 0x0;                    //a_Missing a mandatory period
  }

  return 0x1;                       //a_Passed all tests
}

int ACGI::cgiIsWithoutMetaChar(const char *pccTest, int iStrict)
{
  //a_NULL string or no string
  if (!pccTest || *pccTest == '\x0') return 0x0;
  
  //a_Strict form doesn't allow double-quotes
  const char *pTestSet = (iStrict ? ";<>|&?*$#\'\"" : ";<>&?*|$#\'");
  
  register int iX = strlen(pccTest);
  while (--iX >= 0x0)
  {
    //a_Control characters are not accepted (signed < 0x0 is non-ASCII)
    if (*(pccTest +iX) < 0x20) return 0x0;  
    
    //a_Search for invalid shell metacharacters (these should not be in an image URL)
    if (strchr(pTestSet, *(pccTest + iX))) return 0x0;
  }

  return 0x1;                   //a_No meta characters that can cause problems
}

//a_Checks for invalid metacharacters and quote pairs
int ACGI::cgiIsValidHTMLTag(const char *pccTest)
{
  //a_NULL string or no string
  if (!pccTest || *pccTest == '\x0') return 0x0;

  //a_Check for metacharacters
  if (!cgiIsWithoutMetaChar(pccTest)) return 0x0;

  //a_(Check for quote pairs!!!)
  register int iX;
  const char *pccX = strchr(pccTest, '\"');
  if (pccX)
  {
    //Found one quote
    iX = 0x1;
    while (pccX = strchr(++pccX, '\"'))
    {
      iX++;
    }
  
    if ((iX % 0x2) != 0x0)
      return 0x0;                  //a_Uneven # of quotes
  }

  return 0x1;                      //a_Valid it seems
}

//a_Checks for valid URL (Usually after decoding if neccessary)
//a_  0: NULL, empty string or meta character contained
//a_  1: http://   2: ftp://  3: gopher://
//a_  4: mailto:   5: news:
int ACGI::cgiIsValidURLProtocol(const char *pccTest)
{
  //a_NULL string or no string
  if (!pccTest || *pccTest == '\x0') return 0x0;

  //a_Check for metacharacters, strict test
  if (!cgiIsWithoutMetaChar(pccTest, 0x1)) return 0x0;

  AURL urlTest(pccTest);

  return urlTest.urlIsValidProtocol();
}

//a_Checks if the URL is "valid"
int ACGI::cgiIsValidURL(const char *pccTest)
{
  //a_NULL string or no string
  if (!pccTest || *pccTest == '\x0') return 0x0;

  //a_Check for metacharacters, strict test
  if (!cgiIsWithoutMetaChar(pccTest, 0x1)) return 0x0;

  AURL urlTest(pccTest);

  return urlTest.urlIsValidURL();
}

//a_Scans the string for HTML entries that may be questionable
//a_Usually these will cause problems if embedded into HTML
int ACGI::cgiIsNotValidHTMLLine(const char *pccTest)
{
  //a_NULL string or no string
  if (!pccTest || *pccTest == '\x0') return 0x0;

  const int iiBC = 22;
  const char *sBadCodes[iiBC] =
  { 
    ".exe", ".cgi", ".pl", "<html", "</html", "<body", "</body",
    "<head", "</head", "<script",
    "<form", "#exec", "cmd=", "<meta", "</title", "<title", "!-",
    "<base", "<link", "applet=", "app=", "command=",
  };

  int iX = 0x0, iLength = strlen(pccTest);

  char *pcTest = aStrDup(pccTest);
  if (pcTest)
  {
    for(iX = 0x0; iX < iLength; iX++)
      *(pcTest + iX) = tolower(*(pcTest + iX));

    for (iX = 0x0; iX < iiBC; iX++)
    {
      if (strstr(pcTest, sBadCodes[iX]))
      {
        iX = 0x0;

        break;
      }
    }
  }

  return (iX == iiBC ? 0x0 : iX);      //a_If end of list, then it is valid
}

//a_Gets acharacter string, if it finds unacceptable code it replaces the
//a_ entire <TAG> with <!--> so it is ignored by the browser
//a_This is used to validate input from a user that may be imbedded into a dynamic page
char *ACGI::cgiValidateHTMLLine(char *pcLine)
{
  if (!pcLine)
    return NULL;
  
  //a_Checks that the line is of valid type
  char *pcStart = pcLine, *pcEnd = NULL;

  //a_1 letter: Italic, Bold, Underline, Strikeout, Quote, Anchor
  //a_2 letter: EMphasis, TeleType, line BReak, AUthor
  //a_3 letter: BIG, IMaGe, SUPerscript, SUBscript, KeyBoarD, PREformatted 
  //a_4 letter: CODE, FONT
  //a_5 letter: SMALL
  //a_6 letter: STRONG, STRIKE
  const int iiTagSize = 0x6;
  const char *psTags[iiTagSize] =
  {
    "ibusqa", "emttbrau", "bigimgsupsubkbdpre", "codefont",
    "small", "strongstrike"
  };

  //a_Search for tags and verify they are ok...
  int iOK;      //a_Not tested
  while (pcStart = strchr(pcStart, '<'))
  {
    iOK = -1;                              //a_Start of test

    if (!(pcEnd = strchr(pcStart, '>')))   //a_Where the tag ends
    {
      //a_Kill the rest of the message, all tags must be closed (eg. <TAG>, not. <TAG )
      *pcStart = 0x0;
      return pcLine;
    }

    pcStart++;   //a_Go inside the tag
    
    //a_Make lower inside tag
    for(char *pc = pcStart; pc < pcEnd; ++pc)
      *(pc) = tolower(*pc);

    //a_First check for unacceptable characters
    char cAfter = *(pcEnd + 0x1);   //a_Work one tag at a time
    *(pcEnd) = ' ';                 //a_Back from '>' to ' ' for testing 
    *(pcEnd + 0x1) = 0x0;
    if (cgiIsValidHTMLTag(pcStart))
    {
      if (*pcStart == '/') pcStart++;         //a_Skip the end of tag marker

      if (iOK < 0x0)
      {
        //a_No decision yet, check HTML formatting
        char sTest[0x8];
        const char *pSet;
        for (int iN = 0x0; iN < iiTagSize; iN ++)
        {
          //a_Check N character tags
          *(sTest + iN + 0x1) = ' ';
          *(sTest + iN + 0x2) = 0x0;
          pSet = psTags[iN];
          while (*pSet)
          {
            strncpy(sTest, pSet, iN + 1);
            if (pcStart == strstr(pcStart, sTest))
            {
              iOK = iN + 0x1;       //a_Found a valid tag
              break;
            }
            pSet += (iN + 0x1);     //a_Advance as many characters
          }  
          if (iOK > 0x0) break;     //a_Already found, out of the for() loop
        }
        if (iOK < 0x0) iOK = 0x0;   //a_No decision at the end is a failure
      }
    }
    else
      iOK = 0x0;

    *(pcEnd) = '>';                 //a_Back from NULL to '>'
    *(pcEnd + 0x1) = cAfter;        //a_Restore the one after

    //a_If at least one correct tag was found then this will be equal to the value of its size
    if (iOK <= 0x0)
    {
      //a_This current tag is invalid, fill it with a comment :)
      *(pcStart++) = '!';
      while (pcStart < pcEnd) *(pcStart++) = '-';
    }
  }

  return pcLine;     //a_No problems if it got this far...
}

void ACGI::cgiEncodeAndOutputURL(const char *pccSource, int iLength)
{
  char *pcOut = g_aCrypto.convertoEncodeURL(pccSource, iLength);
  
  if (pcOut)
    outString(pcOut);

  delete []pcOut;
}

void ACGI::cgiEnvironmentDump(int iFullDump)
{
  htmlTag("br");
  htmlTag("hr", "size=\"4\"");
  htmlTag("br");

  htmlDoTag("h1", "Environment Dump");
  htmlDoTag("h2", "HTTP Specific");

  htmlStartTag("table", "width=\"100%\" border=\"2\"");
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "Accept");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPAccept());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "Accept-Encoding");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPAcceptEncoding());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "Accept-Language");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPAcceptLanguage());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "Accept-Charset");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPAcceptCharset());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "Authorization");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPAuthorization());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "ContentType");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetContentType());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "ContentLength");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetContentLength());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "Cookie");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPCookie());            
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "From");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPFrom());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "If-Match");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPIfMatch());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "If-Modified-Since");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPIfModifiedSince());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "If-None-Match");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPIfNoneMatch());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "If-Range");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPIfRange());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "If-Unmodified-Since");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPIfUnmodifiedSince());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "Max-Forwards");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPMaxForwards());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "Proxy-Authorization");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPProxyAuthorization());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "Pragma");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPPragma());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "QueryString");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetQueryString());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "Range");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPRange());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "Referer");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPReferer());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "RequestMethod");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetRequestMethod());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "TE");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPTE());
    htmlEndTag("tr");
  
    htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "User-Agent");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHTTPUserAgent());
    htmlEndTag("tr");
  htmlEndTag("table");

  htmlDoTag("h2", "FORM data");
    htmlStartTag("table", "width=\"100%\" border=\"2\"");
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "FORM data (body of request)");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", getSafeFormData());
      htmlEndTag("tr");
    htmlEndTag("table");

  htmlDoTag("h2", "Server Specific");
    htmlStartTag("table", "width=\"100%\" border=\"2\"");
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "Annot. Server");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetAnnotationServer());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "ServerName");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetServerName());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "ServerSoftware");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetServerSoftware());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "ServerPort");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetServerPort());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "ServerProtocol");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetServerProtocol());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "GatewayInt");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetGatewayInterface());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "RemoteAddress");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetRemoteAddress());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "RemoteHost");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetRemoteHost());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "RemoteIdent");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetRemoteIdent());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "Path");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetPath());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "PathInfo");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetPathInfo());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "PathTranslated= ");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetPathTranslated());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "ScriptName");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetScriptName());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "SHLVL");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetSHLVL());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "PWD");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetPWD());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
      htmlDoTagEx("th", "align=\"right\"", "LogName");
      htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetLogName());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "User");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetUser());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "Host");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHost());
      htmlEndTag("tr");
    
      htmlStartTag("tr");
        htmlDoTagEx("th", "align=\"right\"", "HostType");
        htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", cgiGetHostType());
      htmlEndTag("tr");
    htmlEndTag("table");

  htmlTag("br");
  htmlTag("hr", "size=\"4\"");
  htmlTag("br");

  if (iFullDump)
  {
    htmlStartTag("pre");

    #ifdef _DEBUG_DUMP_
      outStringN("FORM and QueryString structure dump<br/>");
      dump();
    #else
      outStringCRN("<i>#define _DEBUG_DUMP_</i> and rebuild freeCGI for a full dump...");
    #endif

    htmlEndTag("pre");
  }
}

void ACGI::cgiStartFORM(const char *pccAction, const char *pccMethod)
{
  outStringN("<FORM ACTION=\"");
  if (pccAction) outString(pccAction);
  else           cgiGetScriptName(0x1);
  outStringN("\" METHOD=");
  if (pccMethod) outStringQ(pccMethod);
  else           outStringQ("POST");
  outStringCRN(">");
}

void ACGI::_cgiDoInputType(int iType)
{
  outStringN("<input type=\"");
  switch(iType)
  {
    case FORMINPUT_HIDDEN   : outStringN("hidden\" "); break;
    case FORMINPUT_CHECKBOX : outStringN("checkbox\" "); break;
    case FORMINPUT_RADIO    : outStringN("radio\" "); break;
    case FORMINPUT_TEXT     : outStringN("text\" "); break;
    case FORMINPUT_PASSWORD : outStringN("password\" "); break;
    case FORMINPUT_RANGE    : outStringN("range\" "); break;
    case FORMINPUT_SUBMIT   : outStringN("submit\" "); break;
    case FORMINPUT_RESET    : outStringN("reset\" "); break;
    case FORMINPUT_TEXTAREA : outStringN("textarea\" "); break;
    case FORMINPUT_IMAGE    : outStringN("image\" "); break;
    case FORMINPUT_SCRIBBLE : outStringN("scribble\" "); break;
    case FORMINPUT_FILE     : outStringN("file\" "); break;
    default                 : assert(0x0);  //a_Unknown INPUT type!
  }
}

void ACGI::cgiDoFORMInput(int iType, AElementPairList &eplItems, const char *pccContent, const char *pccExtra)
{
 
  //a_Output the input type first
  _cgiDoInputType(iType);

  eplItems.doOut(this);     //a_Output the parameter list for this input item

  //a_Extra tag info
  if (pccExtra)
    outStringN(pccExtra);

  switch(iType)
  {
    case FORMINPUT_TEXTAREA:
      outStringCRN(">");
      break;

    default:
      outStringCRN("/>");
      break;
  }

  //a_Content goes after the tag
  if (pccContent)
  {
    outStringCR(pccContent);  //a_Content string useful in <input type=...>{pccContent} ...
  }

  //a_Special case for TEXTAREA
  switch(iType)
  {
    case FORMINPUT_TEXTAREA:
      outStringCRN("</TEXTAREA>");
      break;
  }
}

void ACGI::cgiDoFORMInput(int iType, const char *pccName, const char *pccValue, const char *pccContent, const char *pccExtra)
{
 
  //a_Output the input type first
  _cgiDoInputType(iType);

  //a_Do the NAME items
  if (pccName)
  {
    outStringN("name=");
    outStringQ(pccName);
  }

  //a_Do the VALUE items
  if (pccValue)
  {
    outStringN(" value=");
    outStringQ(pccValue);
  }

  //a_Extra tag info
  if (pccExtra)
    outStringN(pccExtra);

  switch(iType)
  {
    case FORMINPUT_TEXTAREA:
      outStringCRN(">");
      break;

    default:
      outStringCRN("/>");
      break;
  }

  //a_Content goes after the tag
  if (pccContent)
  {
    outStringCR(pccContent);  //a_Content string useful in <INPUT TYPE=...>{pccContent} ...
  }

  //a_Special case for TEXTAREA
  switch(iType)
  {
    case FORMINPUT_TEXTAREA : outStringCRN("</TEXTAREA>");
  }
}

DWORD ACGI::cgiGetIP(const char *pccIP)
{
  if (!pccIP) pccIP = cgiGetRemoteAddress();
  char sWork[4];
  DWORD dwRet = 0x0;
  for (register int iX = 0x0; iX < 0x4; iX++)
  {
    int iC = 0x0;
    while (*pccIP && *pccIP != '.' && iC < 0x4) sWork[iC++] = *pccIP++;
    
    if (iC >= 0x4) { assert(0x0); break; }    //a_Something went wrong
    else if (*pccIP == '.') pccIP++;          //a_Advance beyond the '.'
    
    sWork[iC] = '\x0';
    DWORD dwTemp = (DWORD((atoi(sWork))) << (0x18 - 0x8 * iX));
    dwRet |= dwTemp;
  }

  return dwRet;
}

int ACGI::cgiOutputBinary(const char *pccMIMEType, const char *pccMIMESubType, const char *pccFilePath)
{
  if (m_posOut && pccMIMEType && pccMIMESubType && pccFilePath)
  {
    #ifdef _MSC_VER
      ifstream ifBinary(pccFilePath, ios::in|ios::binary);
    #elif defined(__BORLANDC__)
      ifstream ifBinary(pccFilePath, ios::in|ios::bin);
    #else
      //a_UNIX and others don't need the  processing
      ifstream ifBinary(pccFilePath, ios::in);
    #endif


#if defined(__BORLANDC__) || defined(__unix)
    if ((ifBinary.rdbuf())->is_open())        
#else
    if (ifBinary.is_open())
#endif
    {
      //a_Get the length of the file
      ifBinary.seekg(0x0, ios::end);
      int iBytesRead = ifBinary.tellg();

      //a_MIME output
      mimeOut(pccMIMEType, pccMIMESubType, iBytesRead);

      //a_Rewind the file
      ifBinary.seekg(0x0, ios::beg);
      
      //a_Set stream to binary
      #if defined(_MSC_VER)
	      *this << ios::binary;
      #elif defined(__BORLANDC__)
        m_posOut.setf(ios::binary);
      #else
        //Probably UNIX and we do not care, everything is binary!
      #endif

      //Output the file
      char sBuffer[READ_BUFFER_SIZE];
      while (!ifBinary.eof())
      {
        ifBinary.read(sBuffer, READ_BUFFER_SIZE);
        iBytesRead = ifBinary.gcount();
        if (iBytesRead > 0x0)
          m_posOut->write(sBuffer, iBytesRead);
      }
    
      ifBinary.close();
      m_posOut->flush();
    }
    else
    {
      #ifdef _DEBUG_DUMP_
        mimeOut("text", "plain");
        outStringN("Filename: ");
        outString(pccFilePath);
        outStringN(" connot be opened!");
      #endif

      return 0x0;
    }
  }
  else
  {
    //a_You have to specify the MIME type/subtype for the binary output
    //a_and/or a non-NULL file path
    //a_and/or output stream variable is NULL
    assert(0x0);    
  
    return 0x0;
  }

  return 0x1;
}
