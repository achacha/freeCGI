/*
  This CGI will save the IP of the browser client to a file
  Useful when you have a DHCP and need the IP saved on the server
  Simply call this CGI every time the IP is changed (which is not too often hopefully)
*/

#define MY_IP_FILE "../ip.html"

#include <freeCGI.h>

int main()
{
  ACGI acgi;
  acgi.cgiGetFormItems();
  acgi.cgiGetHTTPCookie();
  
  acgi.mimeHTML();
  acgi.htmlStartHTML();
  acgi.htmlDoHEAD("Save IP to file");
  acgi.htmlStartBODY();

  //a_Dump the environment
  acgi << "IP=";
  const char *pcc = acgi.cgiGetRemoteAddress(1);
  acgi << "<br>" << endl;

  ofstream fsData(MY_IP_FILE, ios::out);
#if defined(__BORLANDC__) || defined(__unix)
  if ((fsData.rdbuf())->is_open())
#else
  if (fsData.is_open())
#endif
    {
      fsData << "<html><body>IP=<big>" << (pcc ? pcc : "(null)") << endl;
      fsData << "</big><br></body></html>" << endl;
      fsData.close();
    }  
  else
    {
      acgi << "Unable to open file to save IP<br>" << endl;
    }

  acgi.htmlEndBODY();
  acgi.htmlEndHTML();

  return 0x0;
}
