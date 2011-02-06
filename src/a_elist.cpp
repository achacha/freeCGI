
#include "freeCGI.h"

#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

#ifdef _DEBUG_DUMP_
void AElementPairList::dump(void)
{
  cout << endl << "---START: AElementPairList::dump()---";

  APairList::dump();                 //a_The other parent

  //a_AElementPairList specific
  cout << "m_pbUserData=" << (void *)m_pbUserData;
  cout << " m_wUserDataLength=" << m_wUserDataLength << endl;

  cout << "---END: AElementPairList::dump()---" << endl;
}
#endif

AElementPairList::AElementPairList(AElementPairList *pelSource)
{
  if (pelSource)
  {
    //a_Copy constructor
    _bCopy(*pelSource);
  }
  else
  {
    //a_User data used for being persistent upon returning a (const char *)
    m_pbUserData      = NULL;
    m_wUserDataLength = 0x0;
  }
}

AElementPairList::~AElementPairList(void)
{
  delete []m_pbUserData;     //a_NOTE: Do not use this beyond the scope existance of the object
}

void AElementPairList::_bCopy(const AElementPairList &aeSource)
{
  //a_Do the parent copy
  APairList::_bCopy(aeSource);
}

//a_Encoding and decoding of element members
const BYTE *AElementPairList::elDecodeAndGetUserData(const char *pccKName, int &iLength, int iMethod)
{
  delete []m_pbUserData;
  m_pbUserData = NULL;

  if (pccKName)
  {
    const char *pccEncoded = plGetValueByName(pccKName);

    if (pccEncoded)
    {
      //a_Now decode the entry
      m_pbUserData = (BYTE *)aStrDup(pccEncoded);
      m_wUserDataLength = g_aCrypto.convertoFromAlpha(iMethod, (char **)&m_pbUserData, -1);
    }
  }
  else
    assert(0x0);

  iLength = m_wUserDataLength;
  return m_pbUserData;
}

void AElementPairList::elAddEncoded(const char *pccKName, const BYTE *pcbData, int iLength, int iMethod)
{

  APairItem *ppiX;
  if (!(ppiX = plGetItemByName(pccKName)))
  {
    ppiX = plAddItem(pccKName);
  }

  //a_Set it's value encoded
  if (ppiX)
    ppiX->piSetValueEncoded(pcbData, iLength, iMethod);
  else
    assert(0x0);  //a_Allocation error, or something went wrong
}
