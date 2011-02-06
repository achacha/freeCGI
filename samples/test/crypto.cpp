////////////////////////////////////////////////
// Testing ACrypto classes
//

#include "freeCGI.h"

//a_Arbitrary key...
#define MAIN_KEY       "Pinky & The Brain"  //a_There's a fun key :)
#define MAIN_KEYLENGTH 18                   //a_1 added for the NULL terminator

void testACrypto(const char *pccData, int iDataLength);
void test6Bit(const char *pccData, int iDataLength);

int main()
{
  //a_The following functions are for testing and serve as examples of use
  //a_I used strlen() + 1 to keep the NULL termination as part of data for easy debugging :)
  
  cout << "---------------START: A----------------" << endl;
  testACrypto("A", 2);          //a_Encryption and ASCII encoding
  test6Bit("A", 2);          //a_Encryption and ASCII encoding
  cout << "-----------------END: A----------------" << endl;

  cout << "---------------START: XyZ----------------" << endl;
  testACrypto("XyZ", 4);          //a_Encryption and ASCII encoding
  test6Bit("XyZ", 4);          //a_Encryption and ASCII encoding
  cout << "-----------------END: 0123456789----------------" << endl;

  cout << "---------------START: 0123456789----------------" << endl;
  testACrypto("0123456789", 11);          //a_Encryption and ASCII encoding
  test6Bit("0123456789", 11);          //a_Encryption and ASCII encoding
  cout << "-----------------END: 0123456789----------------" << endl;

  cout << "---------------START: %This!is&a(sample)^string..----------------" << endl;
  testACrypto("%This!is&a(sample)^string..", 28);          //a_Encryption and ASCII encoding
  test6Bit("%This!is&a(sample)^string..", 28);          //a_Encryption and ASCII encoding
  cout << "-----------------END: %This!is&a(sample)^string..----------------" << endl;

  return 0x0;
}

void testACrypto(const char *pccData, int iDataLength)
{
  ACrypto acTest;

  char *pcBuffer = aStrDup(NULL, 256);
  int iLength;
  if (pcBuffer)
  {
     strcpy(pcBuffer, pccData);

     acTest.cryptoSetKey((BYTE *)MAIN_KEY, MAIN_KEYLENGTH);
     cout << pcBuffer << endl;
  
     cout << "\nNone:None\n";
     iLength = acTest.cryptoEncrypt(ACrypto::ectNone, &pcBuffer, iDataLength);
     cout << acTest.cryptoDecrypt(ACrypto::ectNone, &pcBuffer, iLength) << endl;
     cout << pcBuffer << endl;

     strcpy(pcBuffer, pccData);
     cout << "\nXOR_Blitter:None\n";
     iLength = acTest.cryptoEncrypt(ACrypto::ectXOR_Blitter, &pcBuffer, iDataLength);
     cout << acTest.cryptoDecrypt(ACrypto::ectXOR_Blitter, &pcBuffer, iLength) << endl;
     cout << pcBuffer << endl;

     strcpy(pcBuffer, pccData);
     cout << "\nXOR_Convolve:None\n";
     iLength = acTest.cryptoEncrypt(ACrypto::ectXOR_Convolver, &pcBuffer, iDataLength);
     cout << acTest.cryptoDecrypt(ACrypto::ectXOR_Convolver, &pcBuffer, iLength) << endl;
     cout << pcBuffer << endl;

     strcpy(pcBuffer, pccData);
     cout << "\nNone:4Bit\n";
     iLength = acTest.cryptoEncrypt(ACrypto::ectNone|ACrypto::eat4Bit, &pcBuffer, iDataLength);
     cout << pcBuffer << endl;
     cout << acTest.cryptoDecrypt(ACrypto::ectNone|ACrypto::eat4Bit, &pcBuffer, iLength) << endl;
     cout << pcBuffer << endl;

     strcpy(pcBuffer, pccData);
     cout << "\nXOR_Blitter:4Bit\n";
     iLength = acTest.cryptoEncrypt(ACrypto::ectXOR_Blitter|ACrypto::eat4Bit, &pcBuffer, iDataLength);
     cout << pcBuffer << endl;
     cout << acTest.cryptoDecrypt(ACrypto::ectXOR_Blitter|ACrypto::eat4Bit, &pcBuffer, iLength) << endl;
     cout << pcBuffer << endl;

     strcpy(pcBuffer, pccData);
     cout << "\nXOR_Convolve:4Bit\n";
     iLength = acTest.cryptoEncrypt(ACrypto::ectXOR_Convolver|ACrypto::eat4Bit, &pcBuffer, iDataLength);
     cout << pcBuffer << endl;
     cout << acTest.cryptoDecrypt(ACrypto::ectXOR_Convolver|ACrypto::eat4Bit, &pcBuffer, iLength) << endl;
     cout << pcBuffer << endl;

     strcpy(pcBuffer, pccData);
     cout << "\nNone:6Bit\n";
     iLength = acTest.cryptoEncrypt(ACrypto::ectNone|ACrypto::eat6Bit, &pcBuffer, iDataLength);
     cout << pcBuffer << endl;
     cout << acTest.cryptoDecrypt(ACrypto::ectNone|ACrypto::eat6Bit, &pcBuffer, iLength) << endl;
     cout << pcBuffer << endl;

     strcpy(pcBuffer, pccData);
     cout << "\nXOR_Blitter:6Bit\n";
     iLength = acTest.cryptoEncrypt(ACrypto::ectXOR_Blitter|ACrypto::eat6Bit, &pcBuffer, iDataLength);
     cout << pcBuffer << endl;
     cout << acTest.cryptoDecrypt(ACrypto::ectXOR_Blitter|ACrypto::eat6Bit, &pcBuffer, iLength) << endl;
     cout << pcBuffer << endl;

     strcpy(pcBuffer, pccData);
     cout << "\nXOR_Convolve:6Bit\n";
     iLength = acTest.cryptoEncrypt(ACrypto::ectXOR_Convolver|ACrypto::eat6Bit, &pcBuffer, iDataLength);
     cout << pcBuffer << endl;
     cout << acTest.cryptoDecrypt(ACrypto::ectXOR_Convolver|ACrypto::eat6Bit, &pcBuffer, iLength) << endl;
     cout << pcBuffer << endl;

     strcpy(pcBuffer, pccData);
     cout << "\nXOR_ShiftingConvolver:6Bit\n";
     iLength = acTest.cryptoEncrypt(ACrypto::ectXOR_ShiftingConvolver|ACrypto::eat6Bit, &pcBuffer, iDataLength);
     cout << pcBuffer << endl;
     cout << acTest.cryptoDecrypt(ACrypto::ectXOR_ShiftingConvolver|ACrypto::eat6Bit, &pcBuffer, iLength) << endl;
     cout << pcBuffer << endl;
  }

  delete []pcBuffer;
}

void test6Bit(const char *pccData, int iDataLength)
{
  ACrypto acTest;

  char *pcBuffer = aStrDup(NULL, 256);
  int iLength;
  if (pcBuffer)
  {
     strcpy(pcBuffer, pccData);

     acTest.cryptoSetKey((BYTE *)MAIN_KEY, MAIN_KEYLENGTH);
     cout << pcBuffer << endl;

     strcpy(pcBuffer, pccData);
     cout << "\nNone:6Bit\n";
     iLength = acTest.cryptoEncrypt(ACrypto::ectNone|ACrypto::eat6Bit, &pcBuffer, iDataLength);
     cout << pcBuffer << endl;
     char *pcTemp = aStrDup(pcBuffer);
     delete []pcBuffer;
     pcBuffer = pcTemp;
     cout << acTest.cryptoDecrypt(ACrypto::ectNone|ACrypto::eat6Bit, &pcBuffer, iLength) << endl;
     cout << pcBuffer << endl;
  }
  else
    assert(0x0);

  delete []pcBuffer;
}
