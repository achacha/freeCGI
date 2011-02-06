////////////////////////////////////////
//
//  Sample CGI program for process FORM items
//
//    Call it like: http://www.myserver.net/mycgi/path cgi_ex.exe?MSG=Hello&LOC=World
//      the extension can be .cgi, .exe or whatever, depending on your server      
//
//    You can also create an HTML page with a FORM submission and use this to test
//      what you are supposed to receive.
//

#define _DEBUG_DUMP_          //a_Enable for extended output (non-shareware version)
//#define WIN32_TEST_          //a_Enable for local debugging

#define MY_INPUT  "MY_INPUT"
#define MY_SUBMIT "MY_SUBMIT"

#include "freeCGI.h"

int main()
{

  #ifdef WIN32_TEST_
    char *pcTest=aStrDup("MY_INPUT=This%20is%20a%20test&MY_SUBMIT=Submit");
  #endif

  ACGI cgiOut;

  //a_Get all possible form items usinf POST, GET or both
  #ifdef WIN32_TEST_
    cgiOut.cgiGetFormItems(pcTest);
  #else
    cgiOut.cgiGetFormItems();
  #endif

  //a_Start the HTML page
  cgiOut.mimeHTML();                      //a_First ever thing to a browser is a MIME directive
  cgiOut.htmlStartHTML();                 //a_Start the HTML page
  cgiOut.htmlDoHEAD("Sample CGI form");   //a_Do the HEAD and TITLE in one
  
  //a_Start the <BODY>
  cgiOut.htmlStartBODY();

  if (cgiOut.plGetValueByName(MY_SUBMIT))
  {
    //a_Reverse the string (yep, this is the heavy duty processing we need to do :)
    const char *pccValue = cgiOut.plGetValueByName(MY_INPUT);
    if (pccValue)
    {
      char *pcWork = aStrDup(pccValue);
      if (pcWork)
      { 
        pcWork = _strrev(pcWork);
        cout << "Your sting reversed: <B><BIG>" << pcWork << "</B></BIG><BR>" << endl;
      }
      delete []pcWork;
    }
    else
    {
      cout << "<B>Your item was not found.</B><BR>" << endl;
    }
    
    //a_There was a form submission, process input
    cgiOut.htmlStartTag("H1", "ALIGN=CENTER");
    cgiOut << "FORM Submission Items";
    cgiOut.htmlEndTag("H1");

    //a_Form items... doOut is implemented in APairList
    //a_doOut accepts AStreamOutput which is a grand-parent of ACGI,
    //a_so as it turns out it works on itself...
    //a_The output is formatted just like a list of CGI parameters.
    //
    //a_Ok, here is a silly example of what this means in real life,
    //a_You are using something that you learned from your mother and applying
    //a_it to something learned from your father...
    //a_Sort of like sewing up a camping tent?!? :)
    cgiOut << "<P><H2>As submitted</H2><BR>";
    cgiOut.doOut(&cgiOut);
    cgiOut.htmlStartTag("HR");
    cgiOut.htmlStartTag("BR");
  
    //a_Now use the methods in ACGI inherited from both AHTML and AFormList
    //a_to display the list contents
    cgiOut << "<P><H2>After parsing</H2><BR>";
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
  
    //a_Dump the environment
    cgiOut.cgiEnvironmentDump();
  }
  else
  {
    //a_Initial startup, show FORM items
    cgiOut.cgiStartFORM();                  //a_Start FORM tag with ACTION to itself
  
    //a_Input item
    AElementPairList eplInput;
    
    eplInput.plAddItem("NAME", MY_INPUT);
    cgiOut.cgiDoFORMInput(FORMINPUT_TEXT, eplInput);

    cgiOut.cgiDoFORMInput(FORMINPUT_SUBMIT, MY_SUBMIT, "Submit");

    cgiOut.cgiEndFORM();                    //a_End the tag
  }
  
  //a_The End...
  cgiOut.htmlEndBODY();
  cgiOut.htmlEndHTML();

  #ifdef WIN32_TEST_
    delete []pcTest;
  #endif

  return 1;
}
