//
// Very simple Ad Banner Rotator
//

#define _DEBUG_DUMP_				//a_Basic debugging
//#define WIN32_TEST_				  //a_Local debugging
//#define _DEBUG_FULL_				  //a_Extended dump

//a_This is the redirection variable
#define S_REDIRECT "URD"

#include "freeCGI.h"           //a_The hierarchy


int main()
{
  ACGI cgiOut;
  
  int iItems;
  #ifdef WIN32_TEST_
    //a_Get form submissions while testing
    iItems = cgiOut.cgiGetFormItems("URD=http://www.myserver.com/mypath/");   
  #else
    //a_Get form submissions, for real :)
    iItems = cgiOut.cgiGetFormItems();                    
  #endif
  
  //a_Now test if this is a request for an ad or a request for an image
  const char *pccValue;
  if (pccValue = cgiOut.plGetValueByName(S_REDIRECT))
  {
    //a_Request to redirect
    cgiOut << "Location: " << pccValue << "\r\n\r\n";
  }
  else
  {
    #ifdef _DEBUG_FULL_
      cgiOut.mimeHTML();
      cgiOut.htmlStartHTML();
      cgiOut.htmlDoHEAD("Sample inline binary");
      cgiOut.htmlStartBODY();
      int iTotalBytes = 0x0;
    #endif
    
    //a_Temporary read buffer (should be at least 256 bytes)
    char sBuffer[256];

    //a_Open the file
    #ifndef WIN32_TEST_
    strcpy(sBuffer, cgiOut.cgiGetPathTranslated());   //a_Only for the HTTPD
    #endif
   
    //a_This is where randomization of images takes place
    strcat(sBuffer, "/images/some_ad.gif");


//a_Have your pick of jpeg or gif or decide after image is picked
//    if (!cgiOut.cgiOutputBinary("image", "jpeg", sBuffer))
    if (!cgiOut.cgiOutputBinary("image", "gif", sBuffer))
    {
      #ifdef _DEBUG_DUMP_
        cgiOut.mimeOut("text", "plain");
        cgiOut << "Filename: ";
        cgiOut.outString(sBuffer);
        cgiOut << " cannot be opened!";
      #endif

      return 0x0;
    }

  }

  return 0x1;
}




