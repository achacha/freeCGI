//////////////////////////////////////////////////////
//
// Sample program to show the functionality of AURL
//

#define _DEBUG_DUMP_
//#define _DEBUG_FULL_

#include "freeCGI.h"
            
int main()
{
  ACGI cgiOut;
  AURL urlTest;
  
  urlTest.urlSet("www.evil.host.com");
  cgiOut << urlTest << endl;

  urlTest.urlSet("mailto:user@www.evil.host.com");
  cgiOut << urlTest << endl;

  urlTest.urlSet("ftp://username:password@www.evil.host.com:21/Directory.Level0/Homepage/myfile.zip");
  cgiOut << urlTest << endl;

  urlTest.urlSet("http://www.evil.host.com:666/~user/index.html#Reference");
  cgiOut << urlTest << endl;

  urlTest.urlSet("http://user:password@www.evil.host.com:666/Directory.Level0/Homepage/cgi-bin/myscript.cgi?A=0&B=1");
  cgiOut << urlTest << endl;



  urlTest.urlSet("helpo://The.end");
  cgiOut << urlTest << endl;

  urlTest.urlSet("http:.com//hello@/homepage");
  cgiOut << urlTest << endl;

  urlTest.urlSet("//ftp?:Yahoo!");
  cgiOut << urlTest << endl;

  urlTest.urlSet("This is the end... My only friend the end!");
  cgiOut << urlTest << endl;

  return 0x1;
