/////////////////////////////////////////////////////////
//
// Example of ACGI validation routines
//

#include "freeCGI.h"

int main()
{
  ACGI cgiOut;

  char *pcTest = aStrDup(NULL, 512);
  if (pcTest)
  {
    strcpy(pcTest, "http://www.panix.com/");
    cout << "\nTest: " << pcTest << endl;
    cout << "cgiIsValidURL=" << cgiOut.cgiIsValidURL(pcTest) << endl;
    cout << "cgiIsValidURLProtocol=" << cgiOut.cgiIsValidURLProtocol(pcTest) << endl;

    strcpy(pcTest, "mailto:achacha@panix.com");
    cout << "\nTest: " << pcTest << endl;
    cout << "cgiIsValidURL=" << cgiOut.cgiIsValidURL(pcTest) << endl;

    strcpy(pcTest, "wellformed@email.net");
    cout << "\nTest: " << pcTest << endl;
    cout << "cgiIsValidEMail=" << cgiOut.cgiIsValidEMail(pcTest) << endl;

    strcpy(pcTest, "badlyformed@email");
    cout << "\nTest: " << pcTest << endl;
    cout << "cgiIsValidEMail=" << cgiOut.cgiIsValidEMail(pcTest) << endl;

    strcpy(pcTest, "Name ;cat /sys/password.txt > mail -r me@somewhere.com");
    cout << "\nTest: " << pcTest << endl;
    cout << "cgiIsWithoutMetaChar=" << cgiOut.cgiIsWithoutMetaChar(pcTest) << endl;

    strcpy(pcTest, "<IMG SRC=\"unmatched/quote.jpg>");
    cout << "\nTest: " << pcTest << endl;
    cout << "cgiIsValidTag=" << cgiOut.cgiIsValidHTMLTag(pcTest) << endl;
    cout << "cgiIsValidHTMLLine=" << cgiOut.cgiIsNotValidHTMLLine(pcTest) << endl;
    cout << "cgiValidateHTMLLine=" << cgiOut.cgiValidateHTMLLine(pcTest) << endl;

    strcpy(pcTest, "<IMG SRC=\"image/picture.jpg\">");
    cout << "\nTest: " << pcTest << endl;
    cout << "cgiIsValidTag=" << cgiOut.cgiIsValidHTMLTag(pcTest) << endl;
    cout << "cgiIsValidHTMLLine=" << cgiOut.cgiIsNotValidHTMLLine(pcTest) << endl;
    cout << "cgiValidateHTMLLine=" << cgiOut.cgiValidateHTMLLine(pcTest) << endl;

    strcpy(pcTest, "<A HREF=\"http://www.yahoo.com/\" NAME=\"Search\">Yahoo!</A>");
    cout << "\nTest: " << pcTest << endl;
    cout << "cgiIsValidTag=" << cgiOut.cgiIsValidHTMLTag(pcTest) << endl;
    cout << "cgiIsValidHTMLLine=" << cgiOut.cgiIsNotValidHTMLLine(pcTest) << endl;
    cout << "cgiValidateHTMLLine=" << cgiOut.cgiValidateHTMLLine(pcTest) << endl;
  }
  
  delete []pcTest;

  return 0x1;
