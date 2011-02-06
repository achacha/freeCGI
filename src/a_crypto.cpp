
#include "freeCGI.h"

#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

/////////////////////////////////////////////////////////////////////
////
//  Many routines are provided with a purpose:
//    a) Your key is in the compiled executable and not easily accessed by an intruder
//    b) The intruder doesn't know which algorithm you are using
//    c) The intruder doesn't know which if any converter you are using
//    d) The lifetime of the encrypted data is usually session based (which is a short time) 
//
//  XOR_Blitter
//    Takes a keyword, XORs the input with it, simple basic, fast, and insecure
//  XOR_Convolver
//    Takes a keywork, XORs with input with key and shifts by a character and repeats until the end
//    Safer than basic XORing
//  XOR_ShiftingConvolver
//    Works like the convolver, but shifts the key by N bytes depending on input
//    Even safer that the convolver
//

//////////////////////////////////////////////////////////////////////////////////////
//
// Construction/Destruction/Statics/Debug
//
//////////////////////////////////////////////////////////////////////////////////////
ACrypto::ACrypto()
{
  //a_Clear the key
  m_pbKey      = NULL;
  m_iKeyLength = 0x0;
}

ACrypto::~ACrypto()
{
  delete []m_pbKey;
  m_pbKey = NULL;        //a_At this time microsoft's compiler has a bug with the extern key word and dtors...
} 

#ifdef _DEBUG_DUMP_
void ACrypto::dump(void)
{
  //a_Only works if the _DEBUG_DUMP_ flag is set and MIME directive was sent (ie. Content-Type: text/html\n\n)
  cout << endl << "---START: ACrypto::dump()---";
  cout << "  m_pbKey = " << (void *)m_pbKey << endl;
  cout << "m_iKeyLength = " << m_iKeyLength << endl;

  AConverto::dump();
  ARandom::dump();

  cout << "---END: ACrypto::dump()---" << endl;
}
#endif

//////////////////////////////////////////////////////////////////////////////////////
//
// Key Control
//
//////////////////////////////////////////////////////////////////////////////////////
int ACrypto::cryptoSetKey(const BYTE *pcbNewKey, int iLength)
{
  assert(this);

  //a_Out with the old, in with the new...
  delete []m_pbKey;
  m_pbKey = NULL;
  m_iKeyLength = 0x0;

  //a_Initial verification and setup
  if (!pcbNewKey || iLength <= 0x0)
  {
    return 0x0; 
  }

  //a_Allocate buffer for key storage
  if (m_pbKey = aMemDup(pcbNewKey, iLength))
  {
    //a_Allocated succesfully
    m_iKeyLength = iLength;
  }
  else
  {
    m_iKeyLength = 0x0;
    return 0x0;
  }

  return 0x1;
}

//a_Returns the seed used for the generation of the series
long ACrypto::cryptoSetRandomKey(int iSize, long lSeed)
{
  assert(this);

  //a_Delete the old key
  delete []m_pbKey;
  m_pbKey = NULL;
  m_iKeyLength = 0x0;

  if (m_pbKey = rngGenerateRandomArray(lSeed, iSize, &ARandom::rngLEcuyer))
    m_iKeyLength = iSize;

  return lSeed;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// Encryption/Decryption wrapper functions 
//   with Conversion AlphaNumeric to/from BYTE array ALGORITHMS (using parent AConverto class)
//
//////////////////////////////////////////////////////////////////////////////////////
int ACrypto::cryptoEncrypt(UINT uType, char **pcInBlock, int iLength, int iReAllocate)
{
  assert(this);
  if (!pcInBlock)
  {
    assert(0x0);
    return 0x0;
  }

  int iNewLength;
  if (iNewLength = cryptoEncryptOnly(uType, *pcInBlock, iLength))
  {
    //a_Successfully encrypted the stream, now convert to AlphaNumberic string
    //iLength is set by the cryptoEncryptOnly routine and taken as &
    assert(iNewLength > 0x0);
    iNewLength = convertoToAlpha(uType, pcInBlock, iNewLength, iReAllocate);
  }
  else
    assert(0x0);    //a_Could not encrypt

  return iNewLength;
}

int ACrypto::cryptoDecrypt(UINT uType, char **pcInBlock, int iLength, int iReAllocate)
{
  assert(this);
  if (!pcInBlock)
  {
    assert(0x0);
    return 0x0;
  }

  //a_Convert AlphaNumberic to Data
  int iNewLength = convertoFromAlpha(uType, pcInBlock, iLength, iReAllocate);
  if (iNewLength != 0x0)
  {
    cryptoDecryptOnly(uType, *pcInBlock, iNewLength);
  }
  else
    assert(0x0);      //a_Zero length string?!?
  
  return iNewLength;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// Encryption/Decryption wrapper functions
//
//////////////////////////////////////////////////////////////////////////////////////
int ACrypto::cryptoEncryptOnly(UINT uType, char *pcInBlock, int iLength)
{
  assert(this);
  assert(iLength == -1 ? 0x1 : iLength > 0);   //a_If length specified, can't be zero
  if (!pcInBlock)
  {
    assert(0x0);
    return 0x0;
  }

  if (!pcInBlock || !m_pbKey) return iLength;  //a_Must have source and key to encrypt

  //a_If no length user strlen + 0x1 for NULL terminator
  if (iLength == -1) 
    if ((iLength = strlen(pcInBlock) + 0x1) <= 0x0)
      return iLength;

  switch (uType & ectMask)
  {
    case ectXOR_Blitter :
      _StandardXORBlit(pcInBlock, iLength); 
    break;
    
    case ectXOR_Convolver :
      _ConvolvingXOREncrypt(pcInBlock, iLength); 
    break;
    
    case ectXOR_ShiftingConvolver :
      _ShiftingConvolvingXOREncrypt(pcInBlock, iLength); 
    break;

    case ectNone :
    break;

    default:
      assert(0x0);
      //a_Invalid encryption type specified
  }

  return iLength;
}

char *ACrypto::cryptoDecryptOnly(UINT uType, char *pcInBlock, int iLength)
{
  assert(this);
  assert(iLength > 0x0);                       //a_Must have a length, \x0 can occur during encryption
  //a_Valid key must be set and input must not be NULL
  if (!pcInBlock || !m_pbKey || m_iKeyLength <= 0x0)
  {
    assert(0x0);
    return 0x0;
  }
  if (iLength <= 0x0) 
  {
    //a_Cannot decrypt buffer of unknown length. \x0 is a possible value!
    return NULL;
  }

  char *pcRet = NULL;                          //a_Used for return
  switch (uType & ectMask)
  {
    case ectXOR_Blitter :
      pcRet = _StandardXORBlit(pcInBlock, iLength); 
      assert(pcRet);     //a_Has to return the parameter passed, and NULL is not passed
    break;
    
    case ectXOR_Convolver :
      pcRet = _ConvolvingXORDecrypt(pcInBlock, iLength); 
      assert(pcRet);     //a_Has to return the parameter passed, and NULL is not passed
    break;
    
    case ectXOR_ShiftingConvolver :
      pcRet = _ShiftingConvolvingXORDecrypt(pcInBlock, iLength); 
      assert(pcRet);     //a_Has to return the parameter passed, and NULL is not passed
    break;

    case ectNone :
      pcRet = pcInBlock;  //a_No encryption
    break;

    default:
      assert(0x0); //a_Invalid encryption type specified
  }

  return pcRet;
}
  
//////////////////////////////////////////////////////////////////////////////////////
//
// Encryption/Decryption ALGORITHMS
//
//////////////////////////////////////////////////////////////////////////////////////
char *ACrypto::_StandardXORBlit(char *pcInBlock, int iLength)
{
  //a_Valid key must be set and input must not be NULL
  if (!pcInBlock || !m_pbKey || m_iKeyLength <= 0x0)
  {
    assert(0x0);
    return 0x0;
  }

  //a_The wrapper that calls this validates the parameters
  int iBlitLength = __min(m_iKeyLength, iLength), iLeftToBlit = iLength;
  char *pcX = pcInBlock;

  //a_Input block is bigger than the key, blit with key, ofsset byt KEY length and blit again
  while (iLeftToBlit > 0)
  {
    for (register int iX = 0x0; iX < iBlitLength; iX++)
    {
      //a_Simple XOR blit
      *(pcX++) ^= m_pbKey[iX];
    }
    
    iLeftToBlit -= iBlitLength;                                               //a_Adjust to how many block blits remain
    iBlitLength = (iLeftToBlit < iBlitLength ? iLeftToBlit : iBlitLength);    //a_Adjust block size if less if left over
  }
  
  return pcInBlock;
}  

//--------------------------------------------------------------------------------------

char *ACrypto::_ConvolvingXOREncrypt(char *pcInBlock, int iLength)
{
  //a_Valid key must be set and input must not be NULL
  if (!pcInBlock || !m_pbKey || m_iKeyLength <= 0x0)
  {
    assert(0x0);
    return 0x0;
  }

  //a_The wrapper that calls this validates the parameters
  int iPos = 0x0;                                             //a_Position in the input block
  int iBlitLength = __min(m_iKeyLength, iLength - iPos);
  
  while (iPos < iLength)
  {
    //a_Convolve the plaintext with the key
    for (register int iX = 0x0; iX < iBlitLength; iX++)
    {
      //a_Do the XOR blit
      *(pcInBlock + iPos + iX) ^= m_pbKey[iX];
    }
  
    //a_Adjust the sizes as the position moves
    iPos++;
    iBlitLength = __min(m_iKeyLength, iLength - iPos);
  }
  
  return pcInBlock;
}  

char *ACrypto::_ConvolvingXORDecrypt(char *pcInBlock, int iLength)
{
  //a_Valid key must be set and input must not be NULL
  if (!pcInBlock || !m_pbKey || m_iKeyLength <= 0x0)
  {
    assert(0x0);
    return 0x0;
  }

  //a_The wrapper that calls this validates the parameters
  int iPos = iLength - 1;                              //a_Position in the input block
  int iBlitLength = __min(m_iKeyLength, iLength - iPos);
  
  while (iPos >= 0)
  {
    //a_Convolve the plaintext with the key
    for (register int iX = 0x0; iX < iBlitLength; iX++)
    {
      //a_Do the XOR blit
      *(pcInBlock + iPos + iX) ^= m_pbKey[iX];
    }
  
    //a_Adjust the sizes as the position moves
    iPos--;
    iBlitLength = __min(m_iKeyLength, iLength - iPos);
  }

  return pcInBlock;
}

//--------------------------------------------------------------------------------------

char *ACrypto::_ShiftingConvolvingXOREncrypt(char *pcInBlock, int iLength)
{
  //a_Valid key must be set and input must not be NULL
  if (!pcInBlock || !m_pbKey || m_iKeyLength <= 0x0)
  {
    assert(0x0);
    return 0x0;
  }

  //a_The wrapper that calls this validates the parameters
  int iPos = 0x0,                                             //a_Position in the input block
      iKeyShift = 0x0;
  int iBlitLength = __min(m_iKeyLength, iLength - iPos);
  
  while (iPos < iLength)
  {
    //a_Convolve the plaintext with the key
    for (register int iX = 0x0; iX < iBlitLength; iX++)
    {
      //a_Do the XOR blit
      *(pcInBlock + iPos + iX) ^= m_pbKey[(iX + iKeyShift) % m_iKeyLength];
    }
  
    //a_Adjust the sizes as the position moves
    iKeyShift = *(pcInBlock + iPos++);                   //a_New shift value
    iBlitLength = __min(m_iKeyLength, iLength - iPos);   //a_Blit length depends on size of input
  }
  
  return pcInBlock;
}  

char *ACrypto::_ShiftingConvolvingXORDecrypt(char *pcInBlock, int iLength)
{
  //a_Valid key must be set and input must not be NULL
  if (!pcInBlock || !m_pbKey || m_iKeyLength <= 0x0)
  {
    assert(0x0);
    return 0x0;
  }

  //a_The wrapper that calls this validates the parameters
  int iPos = iLength - 0x1,                              //a_Position in the input block
      iKeyShift = (iPos - 0x1 >= 0x0 ? *(pcInBlock + iPos -0x1) : 0x0);
  int iBlitLength = __min(m_iKeyLength, iLength - iPos);
  
  while (iPos >= 0)
  {
    //a_Convolve the plaintext with the key
    for (register int iX = 0x0; iX < iBlitLength; iX++)
    {
      //a_Do the XOR blit
      *(pcInBlock + iPos + iX) ^= m_pbKey[(iX + iKeyShift) % m_iKeyLength];
    }
  
    //a_Adjust the sizes as the position moves
    iKeyShift = (--iPos > 0x0 ? *(pcInBlock + iPos - 0x1) : 0x0);   //a_New key shift
    iBlitLength = __min(m_iKeyLength, iLength - iPos);              //a_Adjust blit length is needed
  }

  return pcInBlock;
}

