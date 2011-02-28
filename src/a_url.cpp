
#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

#include "freeCGI.h"

#ifdef _DEBUG_DUMP_
void AURL::dump(void)
{
  cout << endl << "---START: AURL::dump()---";

  AStreamOutput soWhat;
  doOut(&soWhat);

  cout << endl << "---END: AURL::dump()---" << endl;
}
#endif

void AURL::doOut(AStreamOutput *pasOut) const
{
  assert(pasOut);
  if (!pasOut)
    return;

  if (!urlIsValidURL())
  {
    #ifdef _DEBUG_DUMP_
      cout << "<!--AURL::doOut: Invalid URL-->" << endl;
    #endif

    return;
  }

  APairItem *piX;
  int iProtocol = 0x0;

  //a_First comes the protocol
  piX = plGetItemByName(URL_PROTOCOL);
  if (piX)
    *pasOut << piX->piGetValue();
  else
    *pasOut << URL_PROTOCOL_DEFAULT << "://";

  if (piX)
  {
    iProtocol = urlIsValidProtocol(piX->piGetValue());
    if (URL_ID_MAILTO == iProtocol)
      *pasOut << ":";
    else
      *pasOut << "://";
  }

  //a_Username:Password if any
  piX = plGetItemByName(URL_NAME);
  if (piX)
    *pasOut << piX->piGetValue() << "@";

  //a_Host name/IP (required)
  piX = plGetItemByName(URL_HOST);
  if (piX)
    *pasOut << piX->piGetValue();
  else
    assert(0x0);

  //a_Port # if any
  piX = plGetItemByName(URL_PORT);
  if (piX)
    *pasOut << ":" << piX->piGetValue() << "/";

  //a_Path
  piX = plGetItemByName(URL_PATH);
  if (piX)
  {
    *pasOut << piX->piGetValue();

    //a_Query/Ref if any
    piX = plGetItemByName(URL_PARAM);
    if (piX)
      *pasOut << piX->piGetValue();
  }
  else
    *pasOut << URL_PATH_DEFAULT;
}

void AURL::urlSet(const char *pccURL)
{
  //a_Clear everything
  lKill();

  if (!pccURL)
  {
    //a_Just a clearing
    return;
  }

  //a_Parse the URL
  _urlParseAndSet(pccURL);
}

int AURL::urlIsValidURL() const
{
  //a_Just check if the basics are present, only host name is needed rest will be defaulted
  if (plGetItemByName(URL_HOST))
    return 0x1;

  return 0x0;
}

void AURL::_urlParseAndSet(const char *pccURL)
{
  //a_First find all the query items
  char *pcWork = aStrDup(pccURL);
  char *pcX, *pcY = NULL;

  if (pcWork)
  {
    #ifdef _DEBUG_FULL_
      cout << "<!--AURL::_urlParseAndSet(" << pcWork << ")-->" << endl;
    #endif

    //a_Find the parameters
    pcX = strrchr(pcWork, '?');
    if (pcX)
    {
      plAddItem(URL_PARAM, pcX);
      *pcX-- = 0x0;
    }

    //a_Find the method
    pcY = strchr(pcWork, ':');
    if (pcY)
    {
      //a_We don't need the ':' in the protocol
      //a_Remove the ':' and forward slashes, we don't need them...
      do
      {
        *pcY++ = 0x0;
      }
      while (*pcY == '/');

      if (urlIsValidProtocol(pcWork))
        plAddItem(URL_PROTOCOL, pcWork);
    }

    //a_Find path (if any)
    if (pcY && (pcX = strchr(pcY, '/')))
    {
      if (*(pcX + 0x1))
      {
        plAddItem(URL_PATH, pcX + 0x1);
        *pcX-- = 0x0;
      }
    }

    //a_Find name and password
    if (pcY && (pcX = strchr(pcY, '@')))
    {
      *pcX++ = 0x0;
      plAddItem(URL_NAME, pcY);
    }

    //a_Now separate port and host name/IP
    if (pcY && (pcX = strchr(pcY, ':')))
    {
      //a_We have a port
      *pcX++ = 0x0;
      plAddItem(URL_PORT, pcX);
    }

    //a_Only hostname is present?
    if (!pcY) pcY = pcWork;

    //a_Leftovers must be the host (the most important part)...b
    if (pcY && *pcY)
      plAddItem(URL_HOST, pcY);
    else
      assert(0x0);                //a_Wha happink?
  }
}


//a_Checks for valid URL (Usually after decoding if neccessary)
//a_See defines at the top of this file for the #defines constants
int AURL::urlIsValidProtocol(const char *pccTest) const
{
  if (!pccTest) pccTest = plGetValueByName(URL_PROTOCOL);

  //a_NULL string or no string
  if (!pccTest || *pccTest == '\x0') return 0x0;

  #ifdef _DEBUG_FULL_
    cout << "<!--AURL::_urlIsValidProtocol(" << pccTest << ")-->" << endl;
  #endif

  //a_URLs must have a protocol directive (these are good for now...)
  const int iiBC = 10;

  //a_NOTE: Do not change the order of array without changing defines at the top of this file!
  const char *sProtocols[] =
  {
    "http", "ftp", "file", "gopher", "mailto", "news", "telnet", "tn3270", "rlogin", "wais"
  };

  int iRet = 0x0, iLength = strlen(pccTest);
  char *pcTest = aStrDup(pccTest);
  if (pcTest)
  {
    register int iX, iY;                  //a_MSVC doesn't support for() local nested variables yet...
    for (iX = 0x0; iX < iiBC; iX++)
    {
      for(iY = 0x0; iY < iLength; iY++)
        *(pcTest + iY) = tolower(*(pcTest + iY));

      if (!strncmp(pccTest, sProtocols[iX], strlen(sProtocols[iX])))
      {
        iRet = (iX + 0x1);  //a_Found a valid URL directive

        break;
      }
    }
  }

  return iRet;
}
