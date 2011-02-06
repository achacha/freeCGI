
#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

#include "freeCGI.h"

AStateHeader::AStateHeader(const BYTE *pcbUserData, int iUserDataLength, DWORD dwNewID)
{ 
  if (pcbUserData)
  {
    Init(pcbUserData, iUserDataLength);
  }

  ttTime   = 0x0;              //a_Now it's definitely not Jan 1, 1970... :)
  dwUserID = dwNewID;          //a_If non 0x0, then it must be consistent between creation and submission (user IP ternds not to change during session use...)
  wMark    = SH_MARK;          //a_Always set to this, if not, then something is wrong
  wLength  = 0x0;              //a_User must specify
}

void AStateHeader::Init(const BYTE *pcbUserData, int iUserDataLength, DWORD dwNewID)
{
  if (pcbUserData && iUserDataLength > 0x0)     //a_Must have address and length
  {
    //a_Generate CRC-32
    dwCRC32 = g_aCrypto.convertoCRCasDWORD(AConverto::ecrcRNWCRC, (const char *)pcbUserData, iUserDataLength);

    //a_Get the time
    time(&ttTime);

    //a_Save the ID info
    dwUserID = dwNewID;          //a_If non 0x0, then it must be consistent between creation and submission (user IP ternds not to change during session use...)
  }
  else
    assert(0x0);
}

int AStateHeader::GetElapsedSeconds(void) const
{
  assert(ttTime > 0x0);
  time_t ttNow;
  time(&ttNow);
  return int(difftime(ttNow, ttTime));      //a_Will more that INT_MAX seconds elapse?
}

int AStateHeader::IsEqual(AStateHeader &shSource, int iTimeout)
{
  if (IsValid() && shSource.IsValid())
  {
    if ((dwUserID == shSource.dwUserID) &&
        (dwCRC32  == shSource.dwCRC32))
    {
      if (iTimeout > 0x0)
      {
        //a_Time spane specified so, checkelapsed time
        if (GetElapsedSeconds() < iTimeout)
          return 0x1;
        else
          return 0x0;
      }
      else
        return 0x1;        //a_Time span ignored, all checks out
    }
  }

  return 0x0;
}

//-------------------------------------------------------------------------------------

#ifdef _DEBUG_DUMP_
void APairItem::dump(void)
{
  //a_Only works if the _DEBUG_DUMP_ flag is set and MIME directive was sent (ie. Content-Type: text/html\n\n)
  cout << endl << "---START: APairItem::dump()---";
  ABaseElement::dump();
  ADataItem::dump();
  cout << "m_pcName=" << SAFESTRING(m_pcName) << "  m_pcValue=" << SAFESTRING(m_pcValue) << endl;
  cout << "m_pbUserData=" << (void *)m_pbUserData << "  m_wUserDataLength=" << m_wUserDataLength << endl;
  cout << "---END: APairItem::dump()---" << endl;
}
#endif

APairItem::APairItem(APairItem *ppiSource)
{
  if (ppiSource)
    _bCopy(*ppiSource);               //a_Copy the form item
  else
  {
    m_pcName = m_pcValue = NULL;      //a_Name and Value
    m_pbUserData = NULL;
    m_wUserDataLength = 0x0;
  }

  piSetFlag(PI_QUOTED);               //a_Insert quotes by default
}

APairItem::~APairItem()
{
  delete []m_pcName;
  delete []m_pcValue;
  delete []m_pbUserData;
}


void APairItem::_bCopy(const APairItem &piSource)
{
  ABaseElement::_bCopy(piSource);         //a_Parent copy

  piSet(piSource.piGetName(), piSource.piGetValue());
}

void APairItem::doOut(AStreamOutput *pasOut) const
{
  assert(pasOut);
  if (pasOut && m_pcName)
  {
    pasOut->outString(m_pcName);    //a_Display the name and =... if value is NOT NULL
    if (m_pcValue)
    {
      //a_URL Encode if needed
      char *pcOutput;
      if (piIsFlag(PI_URLENCODED))
        pcOutput = g_aCrypto.convertoEncodeURL(m_pcValue);
      else
        pcOutput = m_pcValue;              //a_Just point to the member
      
      pasOut->outStringN("=");
      if (piIsFlag(PI_QUOTED))
        pasOut->outStringQ(pcOutput);     //a_Quoted
      else
        pasOut->outString(pcOutput);      //a_Un-Quoted

      //a_Cleanup
      if (piIsFlag(PI_URLENCODED))
        delete []pcOutput;
    }
  }
}

//a_Both Name and Value are separated
int APairItem::piSet(const char *pccName, const char *pccValue)
{
  if (!pccName) 
  {
    //a_Erase name/value pair
    delete []m_pcName;
    m_pcName = NULL;

    delete []m_pcValue;
    m_pcValue = NULL;
 
    return 0x0;                 //a_Clear returns 0x0
  }
  
  char *pcNewName, *pcNewValue = NULL;

  pcNewName = aStrDup(pccName);
  if (pccValue) pcNewValue = aStrDup(pccValue);
  
  //a_Memory allocated ok, delete old if any
  if (pcNewName)
  {

    delete []m_pcName;
    delete []m_pcValue;

    m_pcName  = pcNewName;
    m_pcValue = pcNewValue;
  }
  else
  {
    //a_Clean up (if any)
    delete []pcNewValue;
    
    return 0x0;
  }

  return 0x1;
}

void APairItem::piSetValue(const char *pccValue)
{
  char *pcNew = aStrDup(pccValue);

  if (pcNew)
  {
    //a_Replace if successfully allocated
    delete []m_pcValue;
    m_pcValue = pcNew;
  }
} 

//----------------------------------------------------------------------------------

//a_Encoding and decoding of element members
void APairItem::piSetValueEncoded(const BYTE *pcbData, int iLength, UINT uMethod, const BYTE *pcbKey, int iKeyLength)
{
  delete []m_pbUserData;
  m_pbUserData = NULL;

  if (pcbData && iLength >= 0x0)
  {
    m_pbUserData = aMemDup(pcbData, iLength);
    m_wUserDataLength = (m_pbUserData ? iLength : 0x0);
  
    //a_Set a key if there is one (NULL will clear key and disable encryption)
    g_aCrypto.cryptoSetKey(pcbKey, iKeyLength);

    //a_Set the user data now ascii encode using AConverto
    assert(iLength > 0x0);
    char *pcX = (char *)aMemDup(pcbData, iLength);
    g_aCrypto.cryptoEncrypt(uMethod, &pcX, iLength);

    piSetValue(pcX);
    
    delete []pcX;
  }
  else
    assert(0x0);    //a_Required variables were incorrect
}

const BYTE *APairItem::piDecodeValueAndGetUserData(int &iLength, UINT uMethod, const BYTE *pcbKey, int iKeyLength)
{
  assert(m_pcName);  //a_Should be named!

  delete []m_pbUserData;
  m_pbUserData = NULL;

  const char *pccEncoded = piGetValue();

  if (pccEncoded)
  {
    //a_Set a key if there is one
    g_aCrypto.cryptoSetKey(pcbKey, iKeyLength);

    //a_Now decode the entry
    m_pbUserData = (BYTE *)aStrDup(pccEncoded);
    m_wUserDataLength = g_aCrypto.cryptoDecrypt(uMethod, (char **)&m_pbUserData, -1);
  }

  iLength = m_wUserDataLength;
  return m_pbUserData;
}


//----------------------------------------------------------------------------------
//a_State variable setting and getting and such
//a_FORMAT for the checked variable's header in AStateHeader
//a_User data passed as a string should include NULL-terminator in the length...
void APairItem::piSetValueChecked(const BYTE *pcbUserData, int iUserDataLength, DWORD dwUserID, UINT uMethod, const BYTE *pcbKey, int iKeyLength)
{
  if (!pcbUserData)
  {
    assert(0x0);
    return;
  }

  delete []m_pbUserData;
  m_pbUserData = NULL;

  if (pcbUserData && iUserDataLength >= 0x0)
  {
    m_pbUserData = aMemDup(pcbUserData, iUserDataLength);
    m_wUserDataLength = (m_pbUserData ? iUserDataLength : 0x0);

    //a_We have a new key dependedent on the dwIP parameter
    //a_Now we need to generate the header info for the user data
    AStateHeader shUserData(pcbUserData, iUserDataLength, dwUserID);
    shUserData.wLength = WORD(iUserDataLength);

    //a_Output string in [0x00, 0xFF] set
    BYTE *pbOut = aMemCat((const BYTE *)&shUserData, sizeof(AStateHeader), pcbUserData, iUserDataLength);
    int iOutLength = sizeof(AStateHeader) + iUserDataLength;

    //a_Store the binary data as ASCII encoded
    piSetValueEncoded(pbOut, iOutLength, uMethod, pcbKey, iKeyLength);

    //a_Cleanup
    delete []pbOut;
  }
  else
    assert(0x0);    //a_Required variables were incorrect
}

const BYTE *APairItem::piDecodeCheckedValueAndGetUserData(int &iUserDataLength, DWORD dwUserID, int iTimeout, UINT uMethod, const BYTE *pcbKey, int iKeyLength)
{
  const BYTE *pcbSecuredData, *pcbUserData = NULL;

  if (pcbSecuredData = piDecodeValueAndGetUserData(iUserDataLength, uMethod, pcbKey, iKeyLength))
  {
    //a_Decoded the value
    AStateHeader shStored, shGenerated;
    if (iUserDataLength > sizeof(AStateHeader))
    {
      //a_Long enough to have the header info
      memcpy(&shStored, pcbSecuredData, sizeof(AStateHeader));
      pcbUserData = pcbSecuredData + sizeof(AStateHeader);
      iUserDataLength -= sizeof(AStateHeader);

      //a_Should be equal to stored value or 1 more (if odd length)
      if (CEILN(iUserDataLength, 0x2) != CEILN(shStored.wLength, 0x2))
        return NULL;                            //a_Bad length
      
      //a_True langth is stored, and my be 1 less if the length is odd (2 BYTE encoding)
      iUserDataLength = shStored.wLength;

      //a_Now generate user data header for the stored data and compare
      shGenerated.Init(pcbUserData, iUserDataLength, dwUserID);
      if (shGenerated.IsEqual(shGenerated, iTimeout))   //a_100 second time lapse
      {
        //a_Header info equal
        return pcbUserData;         //a_Pointer inside m_pbUserData memeber
      }
    }
  }

  return NULL;                     //a_Invalidity!
}
