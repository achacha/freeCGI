#define _DEBUG_DUMP_

#include "freeCGI.h"

#define CK_SUBMIT  "Submit"
#define CK_RFC2109 "RFC2109"

int main()
{
  ACGI cgiOut;

  ACookie acState;
  
  //a_Get submission information
  int iFormItems = cgiOut.cgiGetFormItems();
      iFormItems += cgiOut.cgiGetQueryStringItems();

  int iRemove = -1;   //a_Flag to let us know what we jst did (since this is done before the header is sent)
  if (cgiOut.plGetValueByName(CK_SUBMIT))
  {
    if (strcmp(cgiOut.plGetValueByName(CK_SUBMIT), "Remove") == 0)
    {
      //a_Submitted from form, delete cookie by expiring it
      if (cgiOut.plGetValueByName(CK_RFC2109))
      {
        //a_RFC2109 cookie spec
        iRemove = 2;
        acState.plAddItem("MYNAME", "Alex");
        acState.plAddItem("Version", "1");
        acState.plAddItem("Max-Age", "0");
      }
      else
      {
        //a_Old style Netscape cookie expiration
        iRemove = 1;
        acState.plAddItem("MYNAME", "Alex");
        acState.plAddItem("EXPIRES", "Tue, 01-Jan-1980 00:00:01 GMT");
      }
    }
    else
    {
      if (cgiOut.plGetValueByName(CK_RFC2109))
      {
        //a_RFC2109 cookie spec set cookie
        iRemove = 3;
        acState.plAddItem("MYNAME", "Alex");
        acState.plAddItem("Version", "1");
        acState.plAddItem("Max-Age", "10000");
      }
      else
      {
        //a_Old style Netscape set cookie
        iRemove = 4;
        acState.plAddItem("MYNAME", "Alex");
        acState.plAddItem("EXPIRES", "Friday, 21-MAY-2019 00:00:01 GMT");
      }
    }
  }
  
  //a_DISPLAY STARTS HERE
  cgiOut.mimeHTML(&acState);

  //a_Do initial screen
  cgiOut.htmlStartHTML();
  cgiOut.htmlDoHEAD("Cookie test");
  cgiOut.htmlStartBODY();

  cgiOut << "Current action: " << cgiOut.plGetValueByName(CK_SUBMIT) << " = " << iRemove << " = ";
  switch(iRemove)
  {
    case 1 : cgiOut << "Remove with old Netscape style expire<br/>" << endl; break;
    case 2 : cgiOut << "Remove as per RFC-2109 MaxAge expire<br/>" << endl; break;
    case 3 : cgiOut << "Cookie set with old Netscape style expire<br/>" << endl; break;
    case 4 : cgiOut << "Cookie set as per RFC-2109 MaxAge <br/>" << endl; break;
    default  : cgiOut << "Nothing<br/>" << endl; break;
  }

  cgiOut << "<br/>HTTP_COOKIE=" << cgiOut.cgiGetHTTPCookie();
  cgiOut << "<br/>Current cookie=" << acState << "<br/>" << endl;
  cgiOut << "Display browser cookies: <form><input type=\"button\" value=\"Display current cookies\" onClick=\"javascript:alert(document.cookie);\"></form><br/>" << endl;
  cgiOut.cgiStartFORM(NULL, "GET");   //a)Self posting form
  

  cgiOut << "<HR>" << endl;


  cgiOut << "<input type=\"checkbox\" name=\"" << CK_RFC2109 << "\">Use RFC2109? - ";
  cgiOut << "<br/>Remove cookie: <input type=\"submit\" name=\"" << CK_SUBMIT << "\" value=\"Remove\">" << endl;
  cgiOut << "<br/>Set cookie: <input type=\"submit\" name=\"" << CK_SUBMIT << "\" value=\"Set\">" << endl;

  cgiOut.cgiEndFORM();

  cgiOut << "<hr/>" << endl;
  cgiOut << "AForm::doOut: ";
  cgiOut << cgiOut;
  cgiOut << "<hr/>" << endl;

  cgiOut.cgiEnvironmentDump(0x1);

  cgiOut.htmlEndBODY();
  cgiOut.htmlEndHTML();

  return 0;
}

