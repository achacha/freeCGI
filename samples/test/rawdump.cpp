//////////////////////////////////////////////////
//
// Simplest CGI, just dump the environment variables
//

#define _DEBUG_DUMP_

#include "freeCGI.h"

main ()
{
  ACGI cgiOut;

  //a_First determine that it's a multi-part
  //a_NOTE: I am ignoring the boundary since that must be set up by you
  //a_       and likewise parsed depending on what you expect in the form
  if (strnicmp(cgiOut.cgiGetContentType(), "multipart/form-data", 19) == 0x0)
  {
    cgiOut.mimeHTML();

    cgiOut.htmlStartHTML();                      //a_Start the HTML page
    cgiOut.htmlDoHEAD("form/multipart input");   //a_Do the HEAD and TITLE in one
  
    //a_Start the <BODY>
    cgiOut.htmlStartBODY();

    //a_There was a form submission, process input
    cgiOut.htmlStartTag("HR");
    cgiOut.htmlStartTag("H1", "ALIGN=CENTER");
    cgiOut << "Submission Items from form/multipart";
    cgiOut.htmlEndTag("H1");

    //a_Now get the length of the content
    int iLength = atoi(cgiOut.cgiGetContentLength());
    if (iLength > 0)
    {
      //a_Mysterious modifier needed in form/multi-part
      iLength -= 6;

      //a_Output is preformatted
      cgiOut.htmlStartTag("PRE");
      cgiOut << "Starting to read " << iLength << " bytes.\n" << endl;
      
      //a_Here we know that there is content
      //a_Read in the content from standard input
      char *pcX = new char[iLength + 0x1];  //a_Allocate a data buffer
      int iCount = 0x0;
      while ((iCount < iLength) && (cin.ipfx(1)))
      {
        cin.read(&pcX[iCount], 0x1);
        cgiOut << "{" << iCount << ":'" << pcX[iCount] << "'}" << flush;
        if ((iCount % 0xA) == 0x0) cgiOut << endl;
        iCount++;
      }
      pcX[iCount] = '\x0';

      //aOutput the submission
      cgiOut.outString(pcX);                //a_Output with NULL checking
      cgiOut.htmlEndTag("PRE");
      delete []pcX;                         //a_We do't need it anymore
    }

    //a_Dump the environment (so you see what is going on)
    cgiOut.cgiEnvironmentDump(0x1);     //a_Full dump!

    //a_End the <BODY> and <HTML>
    cgiOut.htmlEndBODY();
    cgiOut.htmlEndHTML();
  }
  else
  {
    //a_Now a multipart, let's print out the FORM here (Quick and dirty)
    cgiOut.mimeHTML();

    cgiOut.htmlStartHTML();                                      //a_Start the HTML page
    cgiOut.htmlDoHEAD("File Upload via form/multipart input");   //a_Do the HEAD and TITLE in one
    cgiOut.htmlStartBODY();                                      //a_Start the <BODY>

    //a_Little title
    cgiOut.htmlStartTag("H1", "ALIGN=CENTER");
    cgiOut << "Please select a file using 'Browse' and submit via 'Send The File'";
    cgiOut.htmlEndTag("H1");

    cgiOut << "<FORM ENCTYPE=\"multipart/form-data\" METHOD=\"POST\">" << endl;
    cgiOut << "<TABLE BORDER=2 CELLPADDING=1><TR><TD>Filename</TD><TD><INPUT TYPE=FILE NAME=\"uploadfile\"></TD></TR>" << endl;
    cgiOut << "<TD><INPUT TYPE=SUBMIT VALUE=\"Send The File\"></TD></TR></TABLE>" << endl;

    //a_End the <BODY> and <HTML>
    cgiOut.htmlEndBODY();
    cgiOut.htmlEndHTML();
  }

  return 0x0;
}
