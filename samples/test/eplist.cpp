//a_Simple example of manipulating AElementPairList

#define _DEBUG_DUMP_

#include "freeCGI.h"

int main()
{
  const char *pccValue = "~ ALPHA-_";
  AHTML htmlOut;
  AElementPairList eplTag;

  //a_Add item to the list (URL encoding is mandatory for HTML)
  htmlOut << "plAddItem: URLEncodeTest=\"" << pccValue << "\"" << endl;
  eplTag.plAddItem("URLEncodeTest", pccValue);
  
  //a_Encoded item doesn't get affected by URL encoding, it's already alphanumeric
  htmlOut << "elAddEncoded: ASCII_Encoded=\"" << pccValue << "\"" << endl;
  eplTag.elAddEncoded("ASCII_Encoded", (const BYTE *)pccValue, strlen(pccValue) + 1, AConverto::eat6Bit);

  //a_Do the output
  htmlOut << eplTag << endl;

  //a_Retrieve our encoded data (which can be binary)
  int iLength;
  htmlOut << "elDecodeAndGetUserData=\"" << (const char *)eplTag.elDecodeAndGetUserData("ASCII_Encoded", iLength, AConverto::eat6Bit) << "\"" << endl;
  
  return 0x1;
