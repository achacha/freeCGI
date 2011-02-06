//////////////////////////////////////////////////////////////////
//a_Program to test CRC methods used
//a_Also serves as an example of how to use the CRC functions
//

#define _DEBUG_DUMP_

#include "freeCGI.h"

void TestCRC(void)
{
  //a_Standard string to compare to CHECK value of a polynomial CRC (see doc/crc.txt)
  //const char ccTest[] = "123456789";
  //a_The medical term coal-miner's disease (actual word!)
  // const char ccTest[] = "(Pneumonoultrasiliconovolcanoconiosis) = Longest work in the English language!  Trivia... :)";
  
  char ccTest[1024];
  cout << "Please enter your test string" << endl;
  cin >> ccTest;
  
  int iLength = strlen(ccTest);

  cout << ccTest;

  cout << endl << "--------------XOR CRC---------------";;
  cout << endl << "{XOR 8Bit}     = ";
  cout << hex << int(g_aCrypto.convertoCRCasBYTE(AConverto::ecrcXOR, ccTest, iLength));
  cout << endl << "{XOR 16Bit}    = ";
  cout << hex << g_aCrypto.convertoCRCasWORD(AConverto::ecrcXOR, ccTest, iLength);
  cout << endl << "{XOR 32Bit}    = ";
  cout << hex << g_aCrypto.convertoCRCasDWORD(AConverto::ecrcXOR, ccTest, iLength);

  cout << endl << "----------Modulo Sum CRC-------------";;
  cout << endl << "{MODSUM 8Bit}  = ";
  cout << hex << int(g_aCrypto.convertoCRCasBYTE(AConverto::ecrcModuloSum, ccTest, iLength));
  cout << endl << "{MODSUM 16Bit} = ";
  cout << hex << g_aCrypto.convertoCRCasWORD(AConverto::ecrcModuloSum, ccTest, iLength);
  cout << endl << "{MODSUM 32Bit} = ";
  cout << hex << g_aCrypto.convertoCRCasDWORD(AConverto::ecrcModuloSum, ccTest, iLength);

  cout << endl << "----------Polynomial CRC-------------";;
  cout << endl << "{CRC 8Bit}     = ";        //a_CHECK = AD for Test="123456789"
  cout << hex << int(g_aCrypto.convertoCRCasBYTE(AConverto::ecrcRNWCRC, ccTest, iLength));
  cout << endl << "{CRC 16Bit}    = ";        //a_CHECK = BB3D for Test="123456789"
  cout << hex << g_aCrypto.convertoCRCasWORD(AConverto::ecrcRNWCRC, ccTest, iLength);
  cout << endl << "{CRC 32Bit}    = ";        //a_CHECK = CBF43926 for Test="123456789"
  cout << hex << g_aCrypto.convertoCRCasDWORD(AConverto::ecrcRNWCRC, ccTest, iLength);

  cout << endl << "-----Custom Polynomial CRC-----------";;
  CRCMODEL crcUser;
  
  //a_"CRC-16/CITT" X25 standard
  crcUser.iWidth   = 0x10;
  crcUser.dwPoly   = 0x1021;
  crcUser.dwInit   = 0xFFFF;
  crcUser.bRefIn   = 0x0;
  crcUser.bRefOut  = 0x0;
  crcUser.dwXOROut = 0x0;
  cout << endl << "{CRC-16/CITT/X25s}  = ";        
  cout << hex << g_aCrypto.convertoCRCasWORD(AConverto::ecrcRNWCRC, ccTest, iLength, &crcUser);

  //a_X25 reversed
  crcUser.iWidth   = 0x10;
  crcUser.dwPoly   = 0x0811;
  crcUser.dwInit   = 0xFFFF;
  crcUser.bRefIn   = 0x0;
  crcUser.bRefOut  = 0x0;
  crcUser.dwXOROut = 0x0;
  cout << endl << "{X25r}              = ";        
  cout << hex << g_aCrypto.convertoCRCasWORD(AConverto::ecrcRNWCRC, ccTest, iLength, &crcUser);

  //a_"XMODEM" CRC16
  crcUser.iWidth   = 0x10;
  crcUser.dwPoly   = 0x8408;
  crcUser.dwInit   = 0x0;
  crcUser.bRefIn   = 0x1;
  crcUser.bRefOut  = 0x1;
  crcUser.dwXOROut = 0x0;
  cout << endl << "{XMODEM-CRC16}      = ";        
  cout << hex << g_aCrypto.convertoCRCasWORD(AConverto::ecrcRNWCRC, ccTest, iLength, &crcUser);

  //a_"LHA" CRC16 reversed
  crcUser.iWidth   = 0x10;
  crcUser.dwPoly   = 0x4003;
  crcUser.dwInit   = 0x0;
  crcUser.bRefIn   = 0x1;
  crcUser.bRefOut  = 0x1;
  crcUser.dwXOROut = 0x0;
  cout << endl << "{LHA-CRC16r}        = ";        
  cout << hex << g_aCrypto.convertoCRCasWORD(AConverto::ecrcRNWCRC, ccTest, iLength, &crcUser);

  cout << endl << "-------------------------------------" << endl;
}

int main()
{
  //a_Shows how to generate CRCs
  TestCRC();

  return 0x0;
