
#include "freeCGI.h"

#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

////////////////////////////////////////////////////////
// CRC routines ported from doc/crc.txt (Excellent article on CRCs)
// (Original CRC code written in C by Ross N. Williams (ross@guest.adelaide.edu.au)) 
//

//a_Mapping set; can use ':' to seperate converted strings
//const char AConverto::sm_ccMap[65] = "_0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-";
const char AConverto::sm_ccMap[65] = "0123456789_ABCDEFGHIJKLMNOPQRSTUVWXYZ-abcdefghijklmnopqrstuvwxyz";

AConverto::AConverto(void)
{
  m_pcUserMap = NULL;
}

AConverto::~AConverto()
{
  delete []m_pcUserMap;
  m_pcUserMap = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// Conversion wrapper functions
//
//////////////////////////////////////////////////////////////////////////////////////
int AConverto::convertoFromAlpha(UINT uType, char **pcInBlock, int iLength, int iReAllocate)
{
  switch(uType & eatMask)
  {
    case eat4Bit :
      return _convertoFrom4BitAlpha(pcInBlock, iLength, iReAllocate);

    case eat6Bit :
      return _convertoFrom6BitAlpha(pcInBlock, iLength, iReAllocate);
  
    default :
      return iLength;    //a_No conversion done, return string length
  }
}

int AConverto::convertoToAlpha(UINT uType, char **pcInBlock, int iLength, int iReAllocate)
{
  switch(uType & eatMask)
  {
    case eat4Bit :
      return _convertoTo4BitAlpha(pcInBlock, iLength, iReAllocate);

    case eat6Bit :
      return _convertoTo6BitAlpha(pcInBlock, iLength, iReAllocate);
  
    default :
      return iLength;    //a_No conversion done, return string length
  }
}

void AConverto::convertoSetMap(const char *pccNewMap)
{
  if (pccNewMap)
  {
    if (strlen(pccNewMap) == 64)
    {
      char *pcNew = aStrDup(pccNewMap);
      if (pcNew)
      {
        //a_Replace if allocation succeeds
        delete []m_pcUserMap;
        m_pcUserMap = pcNew;
      }
    }
    else
      assert(0x0);    //a_Invalid map size, must be 64 characters
  }
  else
  {
    delete []m_pcUserMap;
    m_pcUserMap = NULL;
  }
}

//////////////////////////////////////////////////////////////////////////////////////
//
// Conversion to alphanumric ALGORITHMS
//
//////////////////////////////////////////////////////////////////////////////////////
int AConverto::_convertoTo4BitAlpha(char **ppcInBlock, int iLength, int iReAllocate)
{

  if (!ppcInBlock || !*ppcInBlock) 
  {
    assert(0x0);
    return 0x0;
  }

  int iNewLength = iLength * 0x2;
  char *pcIn = *ppcInBlock,
       *pcNew = aStrDup(NULL, iNewLength + 0x1);   //Ceiling like function (doubles the size)
  if (pcNew)
  {
    register int iX = 0x0;
    while (iX < iLength * 2)
    {
      pcNew[iX]        = ((*pcIn >> 0x4) & '\x0F');
      pcNew[iX]       += (pcNew[iX] > '\x9' ? ('A' - '\xA') : '0');
      pcNew[iX + 0x1]  = *pcIn & '\x0F';
      pcNew[iX + 0x1] += (pcNew[iX + 0x1] > '\x9' ? ('A' - '\xA') : '0');
      
      //a_On with the bodycount...
      iX += 2;
      pcIn++;
    }
    
    pcNew[iX] = 0x0;                //a_NULL terminate
    
    if (iReAllocate)
    {                              
      delete [](*ppcInBlock);       //a_Delete the old block
      *ppcInBlock = pcNew;          //_New block assigned to old pointer
  
    }
    else
    {
      strcpy(*ppcInBlock, pcNew);   //a_User requested no reallocation
      delete []pcNew;
    }
  }
  else
    iNewLength = iLength;   //a_Allocation failure

  return (iReAllocate ? iNewLength : iLength);             //a_Return new length
}

int AConverto::_convertoFrom4BitAlpha(char **ppcInBlock, int iLength, int iReAllocate)
{
  if (!ppcInBlock || !*ppcInBlock) 
  {
    assert(0x0);
    return 0x0;
  }

  if (iLength < 0x0) iLength = strlen(*ppcInBlock);
  if (iLength <= 0x0)
  {  
      assert(0x0);
      return 0x0;       
  
  }
  assert(iLength % 0x2 == 0x0);   //a_Hex set is even count
  
  int iNewLength = (iLength + 0x1) / 0x2;
  char *pcNew = aStrDup(NULL, iNewLength + 0x1);
  register int iX = 0x0;
  if (pcNew)
  {
    char cHi, cLo;
    while ((cHi = *(*ppcInBlock + iX * 0x2)) && (cLo = *(*ppcInBlock + iX * 0x2 + 0x1)))
    {
      //a_Convert from 2 char hex to a BYTE
      if (isxdigit(cHi) && isxdigit(cLo))
      {
        if (isdigit(cHi)) cHi -= '0';
        else cHi = toupper(cHi) - 'A' + '\xA';

        if (isdigit(cLo)) cLo -= '0';
        else cLo = toupper(cLo) - 'A' + '\xA';

        pcNew[iX++] =  cHi * '\x10' + cLo; 
      }
    }

    pcNew[iX] = 0x0;                //a_NULL terminate
    
    if (iReAllocate)
    {
      delete [](*ppcInBlock);       //a_Delete old block
      *ppcInBlock = pcNew;          //a_Assign new block to old pointer
    }
    else
    {
      strcpy(*ppcInBlock, pcNew);   //a_User requested no reallocation...
      delete []pcNew;
    }
  }
  else
    iNewLength = iLength;           //a_No reallocation done

  return (iReAllocate ? iNewLength : iLength);     //a_Return the buffer length
}

int AConverto::_convertoTo6BitAlpha(char **ppcInBlock, int iN, int iReAllocate)
{
  if (!ppcInBlock || !*ppcInBlock || iN <= 0x0) 
  {
    assert(0x0);
    return 0x0;
  }

  const char *pccMap = (m_pcUserMap ? m_pcUserMap : sm_ccMap);       //a_If user set the map, use it

  int iT = iN >> 0x1,                             //a_Quick divide by 2, how many pairs to do
      iR = iN % 0x2,                              //a_Remainder of the division above
      iNewLength = (iT * 0x3 + iR * 0x2 + 0x1);   //a_3 for every 2 BYTEs, 2 for every BYTE leftover and NULL terminator
  
  char *pcNewBuffer = aStrDup(NULL, iNewLength),  //a_Work buffer         
       *pcNew = pcNewBuffer,                      //a_Pointer to the work buffer
       *pcPos = *ppcInBlock;                      //a_Pointer to the source

  if (pcNew)
  {
    WORD wValue, wX;
    register int iX;                              //a_ANSI 2.0 compliance for loop variables
    for (iX = 0x0; iX < iT; iX++)
    {
      //a_Create the 2 BYTEs for easy extraction of bits
      wValue = BYTE(*pcPos++);
      wX = (WORD(*pcPos++) << 8);
      wValue |= (wX & 0xFF00);

      //a_Extract bits and map it
      *pcNew++ = pccMap[wValue & 0x003F];
      *pcNew++ = pccMap[(wValue >> 0x6) & 0x003F];
      *pcNew++ = pccMap[(wValue >> 0xC) & 0x000F];
    }

    //a_Do the remainder
    if (iR == 0x1)
    {
      //a_Do the bottom 6 bits, and see if the top 2 bits are needed
      wValue  = BYTE(*pcPos);
      *pcNew++ = pccMap[wValue & 0x003F];
      //a_Need 2 BYTEs, all 8 bits needed
      wValue >>= 0x6;
      *pcNew++ = pccMap[wValue & 0x0003];
    }
    else
      assert(iR == 0x0);
    
    *pcNew = 0x0;                         //a_NULL terminate
    if (iReAllocate)
    {
      delete [](*ppcInBlock);             //a_Delete old block
      *ppcInBlock = pcNewBuffer;          //a_Assign new block to old pointer
    }
    else
    {
      memcpy(*ppcInBlock, pcNewBuffer, iNewLength);   //a_User requested no reallocation...
      delete []pcNewBuffer;
    }
  }
  else
    iNewLength = iN;                     //a_Allocation failure

  return iNewLength - 0x1;               //a_Return new length (NULL terminator is NOT included in the length)
}

int AConverto::_convertoFrom6BitAlpha(char **ppcInBlock, int iN, int iReAllocate)
{
  if (!ppcInBlock || !*ppcInBlock) 
  {
    assert(0x0);
    return 0x0;
  }

  if (iN < 0x0) iN = strlen(*ppcInBlock);
  if (iN <= 0x0)
    return 0x0;
  
  const char *pccMap = (m_pcUserMap ? m_pcUserMap : sm_ccMap);  //a_If user set the map, use it
 
  int iT = iN / 0x3,                                    //a_Sorry no shifting here :)
      iR = iN % 0x3,                                    //a_Remained of BYTES over the tertiary boundary
      iNewLength = (iT * 0x2 + (iR > 0x0 ? 0x1 : 0x0)); //a_2 BYTEs for every 3 and if there is a remained 1 BYTE for that

  char *pcNewBuffer = aStrDup(NULL, iNewLength),        //a_Work buffer
       *pcNew = pcNewBuffer,                            //a_Pointer to the work buffer
       *pcPos = *ppcInBlock;                            //a_Pointer to the source

  
  if (pcNew)
  {

    WORD wValue;
    register int iX;
    for (iX = 0x0; iX < iT; iX++)
    {
      //a_Map 3 chars to 2 BYTEs
      wValue  = _convertoMap6BitAlphaToWORD(*pcPos++, pccMap);
      wValue |= _convertoMap6BitAlphaToWORD(*pcPos++, pccMap, 6);
      wValue |= _convertoMap6BitAlphaToWORD(*pcPos++, pccMap, 12);

      *pcNew++ = char(wValue         & '\xFF');
      *pcNew++ = char((wValue >> 8)  & '\xFF');
    }

    if (iR > 0x0)
    {
      //a_We have a remainder
      assert(iR == 0x2);

      wValue  = _convertoMap6BitAlphaToWORD(*pcPos++, pccMap);
      wValue |= _convertoMap6BitAlphaToWORD(*pcPos, pccMap, 6);

      *pcNew++ = char(wValue & '\xFF');
    }

    if (iReAllocate)
    {
      delete [](*ppcInBlock);           //a_Delete old block
      *ppcInBlock = pcNewBuffer;        //a_Assign new block to old pointer
    }
    else
    {
      memcpy(*ppcInBlock, pcNewBuffer, iNewLength); //a_User requested no reallocation...
      delete []pcNewBuffer;
    }
  }
  else
    iNewLength = iN;                     //a_Allocation failure

  return iNewLength;                    //a_Return the size of the buffer
}


/////////////////////////////////////////////////////////////////////
// a_hex to BYTE and vice versa
/////////////////////////////////////////////////////////////////////
//a_Converts 2 digit hex to a value 0-0xFF
BYTE AConverto::convertoHEXtoBYTE(char cHi, char cLo)
{
  if (isxdigit(cHi) && isxdigit(cLo))
  {
    if (isdigit(cHi)) cHi -= '0';
    else cHi = toupper(cHi) - 'A' + '\xA';

    if (isdigit(cLo)) cLo -= '0';
    else cLo = toupper(cLo) - 'A' + '\xA';

    return cHi * '\x10' + cLo; 
  }

  return '~';      //a_Some dummy character  
}

//a_Converts a value 0-0xFF to a 2 digit hex string
char *AConverto::convertoBYTEtoHEX(BYTE byteV, char *sRet)
{
  if (sRet)
  {
    //a_Do the convert, base 16 a BYTE will always be 2 characters wide...
    char cT = char(byteV / 0x10),
         cR = char(byteV % 0x10);

    cT += (cT > '\x9' ? ('A' - '\xA') : '0');
    cR += (cR > '\x9' ? ('A' - '\xA') : '0');

    sRet[0] = cT;
    sRet[1] = cR;
    sRet[2] = 0x0;
  }
  return sRet;                           //a_For convenience
}

//a_Increment a 2 BYTE hex # [00-FF]
char *AConverto::convertoHEXplusplus(char *pcRet)
{
  char sWork[4];
  strncpy(sWork, pcRet, 0x2);
  sWork[0x3] = '\x0';
  if (isxdigit(sWork[0x0]) && isxdigit(sWork[0x1]))
  {
    BYTE bV = BYTE((atoi(sWork) + 0x1) & 0xFF);
    convertoBYTEtoHEX(bV, sWork);
    strncpy(pcRet, sWork, 0x2);                  //a_Paste in the increment
  }
  else
  {
    strncpy(pcRet, "00", 0x2);
  }

  return pcRet;
}

/////////////////////////////////////////////////////////////////////
// a_URL encoding and decoding
////////////////////////////////////////////////////////////////////
char *AConverto::convertoEncodeURL(const char *pccSource, int iSourceLength)
{
  char *pcRet = NULL;

  if (pccSource)
  {
    //a_Get length if none specified
    if (iSourceLength < 0x0)
      iSourceLength = strlen(pccSource);

    register int iX, iDest = iSourceLength;
    
    //a_Determine how long the output will be (first pass)
    for (iX = 0x0; iX < iSourceLength; iX++)
      if (!isalnum(pccSource[iX])) iDest += 0x2;  //a_Will have to be expanded to %xx form

    //a_Allocate the new string and encode
    if (pcRet = aStrDup(NULL, iDest))
    {
      //a_Encode (second pass)
      char sHex[3];
      sHex[2] = '\x0';
      iDest = 0x0;
      for (iX = 0x0; iX < iSourceLength; iX++)
      {
        if (!isalnum(pccSource[iX]))
        {
          //a_Convert to hex
          convertoBYTEtoHEX(BYTE(pccSource[iX]), sHex);
          pcRet[iDest++] = '%';
          pcRet[iDest++] = sHex[0x0];
          pcRet[iDest++] = sHex[0x1];
        }
        else
          pcRet[iDest++] = pccSource[iX];
      }

      pcRet[iDest] = '\x0';               //a_NULL terminate
    }
  }
  else
    assert(0x0);

  return pcRet;
}

char *AConverto::convertoDecodeURL(const char *pccSource, int iZeroTest)
{
  char *pcRet = NULL;

  if (pccSource)
  {
    int iSourceLength = strlen(pccSource), iDest =0x0;
    
    if (pcRet = aStrDup(NULL, iSourceLength))       //a_Another string of same size, why waste time determining how to shrink it
    {
      for (register int iX = 0x0; iX < iSourceLength; iX++)
      {
        if (pccSource[iX] == '%')
        {
          //a_Convert if we have space, hate to get an access exception
          if ((iX + 0x2) < iSourceLength)
          {
            pcRet[iDest] = convertoHEXtoBYTE(pccSource[iX + 0x1], pccSource[iX + 0x2]);
            iX += 0x2;

            //a_Do not advance array if we have a NULL and we are testing for a zero
            //a_NULL can be embedded as %00 and cause unexpected behavior with C char arrays
            if (pcRet[iDest] != '\x0' || !iZeroTest)
              iDest++;
          }
        }
        else
          pcRet[iDest++] = pccSource[iX];
      }
      
      pcRet[iDest] = '\x0';
    }
  }
  else
    assert(0x0);

  return pcRet;
}

//////////////////////////////////////////////////////////////////////
//a_Convert COLORREF to a "#rrggbb" string for HTML
//////////////////////////////////////////////////////////////////////
char *AConverto::convertoCOLORREFtoChar(COLORREF cr, char *sRet)
{
  if (sRet)
  {
    sRet[0] = '#';            //a_Start the madness :)
    convertoBYTEtoHEX(GetRValue(cr), sRet + 1);
    convertoBYTEtoHEX(GetGValue(cr), sRet + 3);
    convertoBYTEtoHEX(GetBValue(cr), sRet + 5);
  }

  return sRet;                          //a_For convenience
}

//////////////////////////////////////////////////////////////////////////
// Data Processing
//////////////////////////////////////////////////////////////////////////
BYTE AConverto::convertoCRCasBYTE(int iType, const char *pccData, int iDataLength, PCRCMODEL pcmUserModel)
{
  if (iDataLength <= 0x0 || !pccData)
  {
    assert(0x0);
    return 0x0;
  }

  BYTE bRet = 0x00;
  switch (iType & ecrcMask)
  {
    case ecrcXOR :
    {
      //a_XOR the entire string into 1 BYTE
      for (register int iX = 0x0; iX < iDataLength; iX++) bRet ^= *(pccData + iX);
    }
    break;

    case ecrcModuloSum :
    {
      //a_Adds to one variable and wraps around the 0xFF BYTE boundary
      for (register int iX = 0x0; iX < iDataLength; iX++) bRet += *(pccData + iX);
    }
    break;

    case ecrcRNWCRC :
    {
      //a_Initialize the model
      if (pcmUserModel) memcpy(&m_crcModel, pcmUserModel, sizeof(CRCMODEL));
      else _convertoInit8BitCRC();

      //a_Initialize the CRCMODEL
      m_crcModel.Init();    //a_Do internal setup after parameters are set

      //a_Use buffer to upadte value in the dwReg member of CRCMODEL
      _convertoDoBlockCRC(pccData, iDataLength);     //a_Do a CRC (NOTE: you can keep calling with buffer chunks too)

      //a_Using the current dwReg, do finalization and return CRC value
      bRet = BYTE0(_convertoGetCurrentCRC());      //a_End it and do output XOR and use lowest BYTE
    }
    break;

    default : assert(0x0);  //a_Unknown type
  }

  return bRet;
}

WORD AConverto::convertoCRCasWORD(int iType, const char *pccData, int iDataLength, PCRCMODEL pcmUserModel)
{
  if (iDataLength <= 0x0 || !pccData)
  {
    assert(0x0);
    return 0x0;
  }

  WORD wRet = 0x00;
  switch (iType & ecrcMask)
  {
    case ecrcXOR :
    {
      //a_XOR the entire string into a WORD, uses a shifting XOR operation
      for (register int iX = 0x0; iX < iDataLength; iX++)
      {
        wRet ^= wRet << 0x8;
        wRet ^= *(pccData + iX);
      }
    }
    break;

    case ecrcModuloSum :
    {
      //a_Adds to one variable and wraps around the 0xFF BYTE boundary
      for (register int iX = 0x0; iX < iDataLength; iX++) wRet += *(pccData + iX);
    }
    break;

    case ecrcRNWCRC :
    {
      //a_Initialize the model
      if (pcmUserModel) memcpy(&m_crcModel, pcmUserModel, sizeof(CRCMODEL));
      else _convertoInit16BitCRC();

      //a_Initialize the CRCMODEL
      m_crcModel.Init();    //a_Do internal setup after parameters are set

      //a_Use buffer to upadte value in the dwReg member of CRCMODEL
      _convertoDoBlockCRC(pccData, iDataLength);     //a_Do a CRC (NOTE: you can keep calling with buffer chunks too)

      //a_Using the current dwReg, do finalization and return CRC value
      wRet = LOWORD(_convertoGetCurrentCRC());      //a_End it and do output XOR and use bottom 16 bits
    }
    break;

    default : assert(0x0);  //a_Unknown type
  }

  return wRet;
}

DWORD AConverto::convertoCRCasDWORD(int iType, const char *pccData, int iDataLength, PCRCMODEL pcmUserModel)
{
  if (iDataLength <= 0x0 || !pccData)
  {
    assert(0x0);
    return 0x0;
  }

  DWORD dwRet = 0x00;
  switch (iType & ecrcMask)
  {
    case ecrcXOR :
    {
      //a_XOR the entire string into a WORD, uses a shifting XOR operation
      for (register int iX = 0x0; iX < iDataLength; iX++)
      {
        dwRet ^= dwRet << 0x8;
        dwRet ^= *(pccData + iX);
      }
    }
    break;

    case ecrcModuloSum :
    {
      //a_Adds to one variable and wraps around the 0xFF BYTE boundary
      for (register int iX = 0x0; iX < iDataLength; iX++) dwRet += *(pccData + iX);
    }
    break;

    case ecrcRNWCRC :
    {
      //a_Initialize the model
      if (pcmUserModel) memcpy(&m_crcModel, pcmUserModel, sizeof(CRCMODEL));
      else _convertoInit32BitCRC();

      //a_Initialize the CRCMODEL
      m_crcModel.Init();    //a_Do internal setup after parameters are set

      //a_Use buffer to upadte value in the dwReg member of CRCMODEL
      _convertoDoBlockCRC(pccData, iDataLength);     //a_Do a CRC (NOTE: you can keep calling with buffer chunks too)

      //a_Using the current dwReg, do finalization and return CRC value
      dwRet = _convertoGetCurrentCRC();             //a_End it and do output XOR
    }
    break;

    default : assert(0x0);  //a_Unknown type
  }

  return dwRet;
}

void AConverto::_convertoInit8BitCRC(void)
{
  m_crcModel.iWidth   = 0x8;          //a_8 bit CRC
  m_crcModel.dwPoly   = 0x65;         //a_I just made this up... couldn't find an 8bit poly
  m_crcModel.dwInit   = 0x0;
  m_crcModel.bRefIn   = 0x1;
  m_crcModel.bRefOut   = 0x1;
  m_crcModel.dwXOROut = 0x0;
}

void AConverto::_convertoInit16BitCRC(void)
{
  //a_Name: "CRC-16"
  m_crcModel.iWidth   = 0x10;           //a_16 bit CRC
  m_crcModel.dwPoly   = 0x8005;         //a_A polynomial :)  see: doc/crc.txt
  m_crcModel.dwInit   = 0x0;
  m_crcModel.bRefIn   = 0x1;
  m_crcModel.bRefOut   = 0x1;
  m_crcModel.dwXOROut = 0x0;
}

void AConverto::_convertoInit32BitCRC(void)
{
   //a_Name:  "CRC-32" used in PKZip, AUTODIN II, Ethernet, and FDDI
   m_crcModel.iWidth   = 0x20;        //a_32Bit CRC
   m_crcModel.dwPoly   = 0x04C11DB7;  //a_The tried and true polynomial...
   m_crcModel.dwInit   = 0xFFFFFFFF;
   m_crcModel.bRefIn   = 0x1;
   m_crcModel.bRefOut  = 0x1;
   m_crcModel.dwXOROut = 0xFFFFFFFF;
}
 
void AConverto::_convertoDoBlockCRC(const char *pccBuffer, int iLength)
{
  if (!pccBuffer || iLength < -0x1)
  {
    assert(0x0);              //a_Invalid buffer
    return;
  }
  
  if (iLength < 0x0) iLength = strlen(pccBuffer);   //a_Implies pccBuffer is a valid NULL terminated string

  DWORD dwTopBit = BITMASK(m_crcModel.iWidth - 0x1);
  BYTE bAdd;
  while (iLength-- > 0x0)
  {
    bAdd = *pccBuffer++;

    if (m_crcModel.bRefIn)
      bAdd = convertoMirrorBYTE(bAdd);

    m_crcModel.dwReg ^= ((DWORD)bAdd << (m_crcModel.iWidth - 0x8));
    for (int iX = 0x0; iX < 0x8; iX++)
    {
      if (m_crcModel.dwReg & dwTopBit)
        m_crcModel.dwReg = (m_crcModel.dwReg << 1) ^ m_crcModel.dwPoly;
      else
        m_crcModel.dwReg <<= 1;
      m_crcModel.dwReg &= WIDTHMASK(m_crcModel.iWidth);
    }
  }
}
 
DWORD AConverto::convertoMirrorLowerDWORDBits(DWORD dwSource, int iBits)
{
  DWORD dwWork = dwSource;
  for (register int iX = 0x0; iX < iBits; iX++)
  {
    if (dwWork & 0x1L)
       dwSource |=  BITMASK((iBits - 0x1) - iX);
    else
       dwSource &= ~BITMASK((iBits - 0x1) - iX);
    dwWork >>= 0x1;
  }
  return dwSource;
}

DWORD AConverto::_convertoGetCurrentCRC(void)
{
  //a_Reflects the CRC if needed and XORs it with the output XOR value
  if (m_crcModel.bRefOut)
    return m_crcModel.dwXOROut ^ convertoMirrorLowerDWORDBits(m_crcModel.dwReg, m_crcModel.iWidth);
  else
    return m_crcModel.dwXOROut ^ m_crcModel.dwReg;
}
