/////////////////////////////////////////////////
//
//  Random cryptography example
//

#include "freeCGI.h"

int main()
{
  char *pcBuffer = aStrDup("This is a sample string! - !gnirts elpmas a si sihT");

  if (pcBuffer)
  {
    cout << "Start::pcBuffer  =" << pcBuffer << endl;

    int iLength;
    const BYTE *pbKey;

    //a_Generate a random key of the length of the buffer
    //a_(simulating an "Infinite Pad", the "only" unbreakable entity of encryption :)
    g_aCrypto.cryptoSetRandomKey(strlen(pcBuffer));
    pbKey = g_aCrypto.cryptoGetKey(iLength);
    cout << "Key              =";
    cout.write(pbKey, iLength);
    cout << endl;
    cout << "KeyLength        =" << iLength << endl;

    //a_Encrypt the NULL terminator as well... Use the return value as the length for decryption
    iLength = strlen(pcBuffer) + 0x1;
    iLength = g_aCrypto.cryptoEncryptOnly(ACrypto::ectXOR_ShiftingConvolver, pcBuffer, iLength);
    cout << "Encrypt::pcBuffer=";
    cout.write(pcBuffer, iLength);
    cout << endl;

    //a_Decrypt the buffer
    g_aCrypto.cryptoDecryptOnly(ACrypto::ectXOR_ShiftingConvolver, pcBuffer, iLength);
    cout << "Decrypt::pcBuffer=" << hex << pcBuffer << endl;
  }
  else
    assert(0x0);

  delete []pcBuffer;

  return 0x0;
