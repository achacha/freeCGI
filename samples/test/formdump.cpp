//////////////////////////////////////////////////
//
// Simplest CGI, just dump the environment variables
//

#define _DEBUG_DUMP_

#include "freeCGI.h"

main ()
{
  ACGI cgiOut;
  cgiOut.cgiGetFormItems();

  cgiOut.mimeHTML();

  cgiOut.htmlStartHTML();                     //a_Start the HTML page
  cgiOut.htmlDoHEAD("CGI Form Cycler");   //a_Do the HEAD and TITLE in one
  
  //a_Start the <BODY>
  cgiOut.htmlStartBODY();

  //a_There was a form submission, process input
  cgiOut.htmlStartTag("H1", "ALIGN=CENTER");
  cgiOut << "FORM Submission Items";
  cgiOut.htmlEndTag("H1");

  //a_Form items... doOut is implemented in APairList
  //a_doOut accepts AStreamOutput which is a grand-parent of ACGI,
  //a_so as it turns out it works on itself...
  //a_The output is formatted just like a list of CGI parameters.
  //a_Now use the methods in ACGI inherited from both AHTML and AFormList
  //a_to display the list contents
  if (cgiOut.lGetCount() > 0x0)
  {
    cgiOut.htmlStartTag("PRE");
    cgiOut.doPairs(&cgiOut, ' ', 0x1, 0x0, 0x0); //a_Unquoted, 1 per line, URL unencoded
    cgiOut.htmlEndTag("PRE");
  }
  else
    cgiOut << "No FORM items were submitted.<BR>" << endl;

  //a_Single tags can use Start for <tag>, without ever calling the end which does </tag>...
  cgiOut.htmlStartTag("HR");
  cgiOut.htmlStartTag("BR");

  //a_Do the form stuff here...
  cgiOut.cgiStartFORM();
    cgiOut.cgiDoFORMInput(FORMINPUT_HIDDEN, "PHIDDEN", "1");
    cgiOut.cgiDoFORMInput(FORMINPUT_TEXT, "PDATA");
    cgiOut.cgiDoFORMInput(FORMINPUT_SUBMIT, "POST", "doPOST");
  cgiOut.cgiEndFORM();
  
  cgiOut << "<BR><BR><BR>";

  cgiOut.cgiStartFORM(NULL, "GET");
    cgiOut.cgiDoFORMInput(FORMINPUT_HIDDEN, "GHIDDEN", "1");
    cgiOut.cgiDoFORMInput(FORMINPUT_TEXT, "GDATA");
    cgiOut.cgiDoFORMInput(FORMINPUT_SUBMIT, "GET", "doGET");
  cgiOut.cgiEndFORM();

  cgiOut << "<BR><BR><HR>";

  //a_Dump the environment
  if (cgiOut.plGetValueByName("DUMP"))
    cgiOut.cgiEnvironmentDump(0x1);     //a_Full dump!

  //a_End the <BODY> and <HTML>
  cgiOut.htmlEndBODY();
  cgiOut.htmlEndHTML();

  return 0x0;
}
