
#include "freeCGI.h"

#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

///////////////////////////////////////////////////////////////////////////////////
// Global helper functions
///////////////////////////////////////////////////////////////////////////////////
//a_The xxxDup functions are allocation wrappers, I will implement exception handling very soon!
//a_strdup may not be available on all systems (ie. WIN32)
//a_Examples of use:
//  char *pcX = aStrDup("Sample String");        pcX == "Sample String"
//  char *pcX = aStrDup(NULL, 128);              pcX == "" of allocated length 128
//  char *pcX = aStrDup("Sample String", 6, 1);  pcX == "Sample"  (Allocated length is 7 (1 for NULL)
char *aStrDup(const char *pccNew, int iLength, int iNullTerminate)
{
  char *pcRet = NULL;
  if (iLength > 0x0)
  {
    //a_Fixed length duplicate
    if (iNullTerminate) iLength++;             //a_Copy N characters + 1 for NULL
    
    pcRet = new char[iLength];
    if (pccNew)
    {
      if (iNullTerminate)
      {
        //a_NULL terminate
        memcpy(pcRet, pccNew, iLength - 1);
        pcRet[iLength - 1] = '\x0';            //a_Copy specified length and NULL terminate
      }
      else
      {
        if (pccNew)
          memcpy(pcRet, pccNew, iLength);     //a_Make a memory image
        else
          memset(pcRet, 0x0, iLength);        //a_Using NULL string, just clear it
      }
    }
    else
      *pcRet = '\x0';                         //a_NULL terminate the result
  }
  else
  {
    if (pccNew)
    {
      //a_Only allocate if a new string exists, since the length was not specified
      iLength = strlen(pccNew);
      if (pccNew)
      {
        pcRet = new char[iLength + 0x1]; //a_Length == -1, so use strlen and + 1 for NULL
        strcpy(pcRet, pccNew);
      }
    }
  }

  return pcRet;     //a_Recepient must call delete[]
}

//a_This one takes 2 NULL terminated strings and creates one combined one
char *aStrCat(const char *pcc1, const char *pcc2)
{
  char *pcRet = NULL;
  
  //a_Only 1 is a valid string, other is a NULL
  if (!pcc1 && pcc2)
    return aStrDup(pcc2);
  if (pcc1 && !pcc2)
    return aStrDup(pcc1);

  //a_Concatinate
  if (pcc1 && pcc2)
  {
    int iL1 = strlen(pcc1), iL2 = strlen(pcc2);
    pcRet = aStrDup(NULL, iL1 + iL2);
    if (pcRet)
    {
      memcpy(pcRet, pcc1, iL1);
      memcpy(pcRet + iL1, pcc2, iL2);
      *(pcRet + iL1 + iL2) = '\x0';                             //a_NULL terminate
    }
  }
  else
  {
    assert(0x0);  //a_Both are NULL, that can't be good
    return aStrDup(NULL, 1);   //a_ return ""
  }

  return pcRet;
}

//a_This one takes up to 3 NULL terminated strings and creates one combined one
char *aStrCat3(const char *pcc1, const char *pcc2, const char *pcc3)
{
  char *pcRet = NULL;
  
  //a_If call other versions if some strings are NULL (brute force algorithm)
  if (pcc1)
  {
    if (!pcc2 && pcc3)
      return aStrCat(pcc1, pcc3);
    if (pcc2 && !pcc3)
      return aStrCat(pcc1, pcc2);
    if (!pcc2 && !pcc3)
      return aStrDup(pcc1);
  }
  if (pcc2)
  {
    if (pcc1 && !pcc3)
      return aStrCat(pcc1, pcc2);
    if (!pcc1 && pcc3)
      return aStrCat(pcc2, pcc3);
    if (!pcc1 && !pcc3)
      return aStrDup(pcc2);
  }

  if (pcc3)
  {
    if (pcc1 && !pcc2)
      return aStrCat(pcc1, pcc3);
    if (!pcc1 && pcc2)
      return aStrCat(pcc2, pcc3);
    if (!pcc1 && !pcc2)
      return aStrDup(pcc3);
  }
  //a_Concatinate
  if (pcc1 && pcc2 && pcc3)
  {
    int iL1 = strlen(pcc1), iL2 = strlen(pcc2), iL3 = strlen(pcc3);
    pcRet = aStrDup(NULL, iL1 + iL2 + iL3);
    if (pcRet)
    {
      memcpy(pcRet, pcc1, iL1);
      memcpy(pcRet + iL1, pcc2, iL2);
      memcpy(pcRet + iL1 + iL2, pcc3, iL3);
      *(pcRet + iL1 + iL2 + iL3) = '\x0';        //a_NULL terminate
    }
  }
  else
  {
    assert(0x0);  //a_Both are NULL, that can't be good
    return aStrDup(NULL, 1);   //a_ return ""
  }

  return pcRet;
}

//a_memdup (may contain NULLs, so length is required)
//a_This one will make a duplicate of a raw BYTE array and NULL is never considered
BYTE *aMemDup(const BYTE *pbNew, int iLength)
{
  BYTE *pbRet = NULL;
  if (iLength > 0x0)
  {
    pbRet = new BYTE[iLength];
    if (pbNew)
      memcpy(pbRet, pbNew, iLength);     //a_Using NULL will not copy, just allocate
  }

  return pbRet;     //a_Recepient must call delete[]
}

//a_This one takes 2 buffers and 2 lengths and creates a new combined one
BYTE *aMemCat(const BYTE *pb1, int iL1, const BYTE *pb2, int iL2)
{
  BYTE *pbRet = NULL;
  if (pb1&& pb2 && iL1 > 0x0 && iL2 > 0x0)
  {
    pbRet = aMemDup(NULL, iL1 + iL2);
    if (pbRet)
    {
      //a_Append the second one
      memcpy(pbRet, pb1, iL1);
      memcpy(pbRet + iL1, pb2, iL2);
    }
  }
  else
    assert(0x0);

  return pbRet;
}

