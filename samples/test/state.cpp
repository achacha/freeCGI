/////////////////////////////////////////////////////////////////////
//a_Program to demonstrate the use of state variables in HTML
//
//

#define _DEBUG_DUMP_
//#define _DEBUG_FULL_

#include "freeCGI.h"

#define ST_KEY       (const BYTE *)"My_Sample_Key"
#define ST_KEYLENGTH 0xD

void testCheckedPair(ACGI &cgiOut, const BYTE *pcbData, int iLength, int iTimeout, UINT uMethod, const BYTE *pcbKey = NULL, int iKeyLength = 0x0);

int main()
{
  ACGI cgiOut;

  //a_2 different encoding methods
  testCheckedPair(cgiOut, (const BYTE *)"Sample string 0", 0x11, 100, AConverto::eat4Bit);
  cgiOut << "-----------------------------------------" << endl;

  testCheckedPair(cgiOut, (const BYTE *)"Sample string 1", 0x11, 100, AConverto::eat6Bit);
  cgiOut << "-----------------------------------------" << endl;
  

  //a_With some encryption methods (Shifting convolver will not work due to encoding)
  testCheckedPair(cgiOut, (const BYTE *)"Sample string 11", 0x11, 100, ACrypto::ectXOR_Blitter|AConverto::eat6Bit, ST_KEY, ST_KEYLENGTH);
  cgiOut << "-----------------------------------------" << endl;

  testCheckedPair(cgiOut, (const BYTE *)"Sample string 12", 0x11, 100, ACrypto::ectXOR_Convolver|AConverto::eat6Bit, ST_KEY, ST_KEYLENGTH);
  cgiOut << "-----------------------------------------" << endl;

  return 0x0;
}

void testCheckedPair(ACGI &cgiOut, const BYTE *pcbData, int iLength, int iTimeout, UINT uMethod, const BYTE *pcbKey, int iKeyLength)
{
  //a_Create state data to be saved
  APairItem piTest;
  DWORD dwIP = cgiOut.cgiGetIP("166.17.0.1");  //a_My provider I think :)
  BYTE *pbState = aMemDup(NULL, iLength);     
  memcpy(pbState, pcbData, iLength);           //a_Here the data is ASCII, but it doesn't have to be

  //-----------------------------------------------------------
  //a_Preparing the posting of data
  piTest.piSet("STATE");
  piTest.piSetValueChecked(pbState, iLength, dwIP, uMethod, pcbKey, iKeyLength);
  cgiOut << piTest << endl;
  //a_Data saved in the HTML page for later submission
  //-----------------------------------------------------------

  //a_Save the value to simulate a post
  char *pcValue = aStrDup(piTest.piGetValue());
  piTest.piSet(NULL);    //a_Clear the pair

  //-----------------------------------------------------------
  //a_After receiving the posting of data
  const BYTE *pcbNewData;
  piTest.piSet("STATE", pcValue);
  pcbNewData = piTest.piDecodeCheckedValueAndGetUserData(iLength, dwIP, iTimeout, uMethod, pcbKey, iKeyLength);
  if (pcbNewData)
  {
    memcpy(pbState, pcbNewData, iLength);
    cgiOut << "STATE=" << (const char*)pbState;      //a_I assumed it was ASCII from start...
    cgiOut << "  length=" << iLength << endl;
  }
  else
    cgiOut << "=(null)?" << endl;

  //a_Cleanup
  delete []pcValue;
  delete []pbState;
}

