/*
  This CGI is meant to provide a simple "Hello World" app that can be used to create other CGIs
*/

#include <freeCGI.h>

int main()
{
  ACGI acgi;
  acgi.cgiGetFormItems();
  acgi.cgiGetHTTPCookie();
  
  acgi.mimeHTML();
  acgi.htmlStartHTML();
  acgi.htmlDoHEAD("Sample CGI Program");
  acgi.htmlStartBODY();

  //a_The meat of the page
  acgi << "Hello World" << endl;

  acgi.htmlEndBODY();
  acgi.htmlEndHTML();

  return 0x0;
}
