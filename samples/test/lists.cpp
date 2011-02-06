#define _DEBUG_DUMP_

#include "freeCGI.h"

//a_Basic test of an element
void testElement()
{
  AHTML htmlOut;
  AElement aeX;

  aeX.eSetNAME("MyElement");
  aeX.eSetVALUE("MyElementsValue");

  aeX.flAddItem("KEY", "KEYVALUE");

  htmlOut << aeX << endl;
}

//a_Example/test of AImage element
void testImage()
{
  AHTML htmlOut;
  AImage aiX;

  //a_NOTE: use / as a separator of directories, understood by UNIX, WinNT and Win95... or use \\ if you must...
  //a_NOTE: you have to specify the \" character if you are to includ things in quotes
  htmlOut << "IMG setup initially" << endl;
  aiX.imageSetSRC("\"/images/kitty.jpg\"");       //a_This one is 130K
  aiX.imageSetLOWSRC("\"/images/ii_kitty.jpg\""); //a_This is an inline image 4K
  aiX.imageSetALT("\"-Siamese Cat-\"");
  
  aiX.imageSetSize(300, 250);
  aiX.imageSetISMAP();

  htmlOut << aiX << endl;

  htmlOut << "Changed SRC and removed LOWSRC, ISMAP" << endl;
  aiX.imageSetSRC("\"images/littlekitty.jpg\"");
  aiX.imageSetLOWSRC();  //a_NULL by default
  aiX.imageSetISMAP(0x0);     //a_Removes the flag

  htmlOut << aiX << endl;

  //a_NOTE: AImage is a child of AElement, so AFormItem memebers
  //a_ are usable by it.
}

void testEncoded()
{
  AHTML htmlOut;
  AElement eeX;

  eeX.flAddItem("NORMALKEY", "MyValue...");
  eeX.eAddEncoded("ENCODEDKEY6", "My Encoded Value 6-Bit!", 23, AConverto::eat6Bit);
  eeX.eAddEncoded("ENCODEDKEY4", "My Encoded Value 4-Bit!", 23);

  htmlOut << eeX << endl;

  const char *pcRet;
  int iL;

  pcRet = eeX.eDecodeAndGetUserData("ENCODEDKEY6", iL, AConverto::eat6Bit);
  if (pcRet)
    htmlOut << "Actually: ENCODEDKEY6=" << pcRet << endl;

  pcRet = eeX.eDecodeAndGetUserData("ENCODEDKEY4", iL);
  if (pcRet)
    htmlOut << "Actually: ENCODEDKEY4=" << pcRet << endl;
}

int main()
{
//  testElement();
  testImage();
//  testEncoded();

  return 668;  //a_Return of the neighbor of the beast
