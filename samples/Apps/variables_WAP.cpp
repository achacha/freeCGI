/*
  This CGI is meant to provide a simple "Hello World" app that can be used to create other CGIs
  To be used with WAP compliant browsers/devices
*/

#include <freeCGI.h>

int main()
{
  ACGI acgi;
  acgi.cgiGetFormItems();
  acgi.cgiGetHTTPCookie();
  
  acgi.mimeOut("text", "vnd.wap.wml");
  acgi << "<?xml version=\"1.0\"?>" << endl;
  acgi << "<!DOCTYPE wml PUBLIC \"-//WAPFORUM//DTD WML 1.1//EN\" \"http://www.wapforum.org/DTD/wml_1.1.xml\">" << endl;

  acgi << "<wml>" << endl;
  acgi << "<card id=\"main\" title=\"freeCGI++: The Sample WAP CGI\">" << endl;

  //a_Dump the environment
  acgi.cgiEnvironmentDump();

  acgi << "</card>" << endl;
  acgi << "</wml>" << endl;

  return 0x0;
}
