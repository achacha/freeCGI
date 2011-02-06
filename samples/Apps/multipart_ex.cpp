//////////////////////////////////////////////////
//
// MIME multi-part file upload sample
//
#define _DEBUG_DUMP_
//#define _DEBUG_FULL_

#include "freeCGI.h"

int main ()
{
  ACGI cgiOut;

  //a_First determine that it's a multi-part
  //a_NOTE: I am ignoring the boundary since that must be set up by you
  //a_       and likewise parsed depending on what you expect in the form
  if (strncmp(cgiOut.cgiGetContentType(), "multipart/form-data", 19) == 0x0)
  {
    cgiOut.mimeHTML();

    cgiOut.htmlStartHTML();                      //a_Start the HTML page
    cgiOut.htmlDoHEAD("form/multipart input");   //a_Do the HEAD and TITLE in one
  
    //a_Start the <BODY>
    cgiOut.htmlStartBODY();

    //a_There was a form submission, process input
    cgiOut.htmlTag("hr");
    cgiOut.htmlDoTagEx("h1", "align='center'", "Submission Items from form/multipart");

    //a_Find a boundary  (see RFC1867)
    //a_START_BOUNDARY="--" + boundary
    //a_END_BOUNDARY  ="--" + boundary + "--"
    const char *pccB = strstr(cgiOut.cgiGetContentType(), "boundary=");
    pccB += 9;
    char *pBoundaryStart = aStrCat("--", pccB);
    char *pBoundaryEnd = aStrCat(pBoundaryStart, "--");

    AHTMLHelper::formatBinaryData("BOUNDARY_START", cgiOut, pBoundaryStart);
    AHTMLHelper::formatBinaryData("BOUNDARY_END", cgiOut, pBoundaryEnd);
    
    #ifdef _DEBUG_FULL_
      cgiOut << "BOUNDARY_START: " << pBoundaryStart << "<br/>" << endl;
      cgiOut << "BOUNDARY_END  : " << pBoundaryEnd << "<br/>" << endl;
    #endif

    //a_Now get the length of the content (usually a ballpark since CRLF is handled differently in multipart submitions)
    int iLength = atoi(cgiOut.cgiGetContentLength());
    cgiOut << "ContentLength reported: " << iLength << "<br/>" << endl;

    //a_Here we know that there is content
    //a_Read in the content from standard input
    char *pLine = new char[2048];
    char *pcX = new char[iLength + 256];  //a_Allocate a data buffer plus extra
    *pcX = '\x0';
    int iCount = 0x0;
    bool boolNotDone = true;
    while (boolNotDone)
    {
      *pLine = '\x0';
      cin.getline(pLine, 2048);
      iCount += strlen(pLine);
      strcat(pcX, pLine);
      strcat(pcX, "\r\n");   //a_Make sure every line has a CRLF as required by RFC2616

      if (!strcmp(pLine, pBoundaryEnd))
      {
        cgiOut << "END Boundary found<br/>" << endl;
        boolNotDone = false;
        continue;
      }
      else if (!strcmp(pLine, pBoundaryStart))
      {
        cgiOut << "-- Boundary start found, new content to follow --<br/>" << endl;
      }
      else
      {
        //a_Debugging output
        AHTMLHelper::formatBinaryData("READLINE", cgiOut, pLine, strlen(pLine), 32);
        cgiOut << "<br/>" << endl;
      }  
    }
    delete []pBoundaryStart;
    pBoundaryStart = NULL;
    delete []pBoundaryEnd;
    pBoundaryEnd = NULL;
    delete []pLine;
    pLine = NULL;

    cgiOut << "Data read: " << iCount << "<br/>" << endl;

    //a_Output the submission
    cgiOut.htmlTag("br");
    cgiOut.htmlTag("hr");
    cgiOut.htmlTag("br");
    cgiOut.htmlStartTag("table", "border='1'");
    cgiOut.htmlStartTag("tr");
    cgiOut.htmlStartTag("th", "align='left'");
    cgiOut << "<b>Content-Type</b>: " << cgiOut.cgiGetContentType() << "<br/>" << endl;
    cgiOut << "<b>Content-Length</b>: " << cgiOut.cgiGetContentLength() << "<br/>" << endl;
    cgiOut.htmlEndTag("th");
    cgiOut.htmlEndTag("tr");

    cgiOut.htmlStartTag("tr");
    cgiOut.htmlStartTag("td");
    cgiOut.htmlDoTag("pre", pcX);
    cgiOut.htmlEndTag("td");
    cgiOut.htmlEndTag("tr");
    cgiOut.htmlEndTag("table");

    cgiOut.htmlTag("br");
    cgiOut.htmlTag("hr");
    cgiOut.htmlTag("br");

    AHTMLHelper::formatBinaryData("POST DATA", cgiOut, pcX, iCount, 32);
   
    delete []pcX;                         //a_We do't need it anymore

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
    cgiOut.htmlStartTag("h1", "align=\"center\"");
    cgiOut << "Please select a file using 'Browse' and submit via 'Send The File'";
    cgiOut.htmlEndTag("h1");

    cgiOut << "<form enctype='multipart/form-data' method='post'>" << endl;
    cgiOut << "<table border='2' cellpadding='1'><tr><td>Filename</td><td><input type=file name='uploadfile'></td></tr>" << endl;
    cgiOut << "<td><INPUT TYPE=text name='comment' value='Comment'></td>" << endl;
    cgiOut << "<td><input type=submit value='Send The File'></td></tr></table>" << endl;

    //a_End the <BODY> and <HTML>
    cgiOut.htmlEndBODY();
    cgiOut.htmlEndHTML();
  }

  return 0x0;
}
