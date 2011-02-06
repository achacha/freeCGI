
#define _DEBUG_DUMP_

#include "freeCGI.h"

void testImageElement(AHTML &htmlOut)
{
  //a_Image example
  AElement eImage;
  eImage.eSetTag("IMG");
  eImage.plAddItem("NAME", "IMAGE1");                 
  eImage.plAddItem("VALUE", "VALUE1");
  eImage.plAddItem("ID", "MyTestImage");
  eImage.plAddItem("SRC", "/images/t_plucky.gif");
  eImage.plAddItem("LOWSRC", "/images/tinytoon.jpg");
  eImage.plAddItem("ALT", "Plucky Ducky");
  eImage.plAddItem("WIDTH", "120");
  eImage.plAddItem("HEIGHT", "90");

  htmlOut << eImage << endl;

  eImage.plAddItem("SRC", "/image/babs.gif");
  eImage.plAddItem("ALIGN", "CENTER|TOP");

  htmlOut << eImage << endl;

  eImage.plAddItem("SRC", "/image/hampton.gif");
  eImage.plRemoveItemByName("ALIGN");

  htmlOut << eImage << endl;
}

int main()
{
  ACGI cgiOut;
  cgiOut.mimeHTML();
  cgiOut.htmlDoHEAD("Testing of AElement");
  
  cgiOut.htmlStartBODY();

  //a_Testing of elements and how they work
  testImageElement(cgiOut);

  cgiOut.htmlEndBODY();
  cgiOut.htmlEndHTML();

  //a_The end... :)
  cgiOut << endl;    //a_Flush!
  return 0x0;
