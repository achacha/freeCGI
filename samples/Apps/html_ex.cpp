#define _DEBUG_DUMP_  //a_Comment this out to suppress debugging output

#include "freeCGI.h"

int main()
{
  //a_First allocate the HTML object
  AHTML htmlOut;

  htmlOut.mimeHTML();         //a_Let the browser know that HTML will follow

  //a_Start our HTML page
  htmlOut.htmlStartHTML();

  //a_Now we do the HEAD and TITLE
  htmlOut.htmlDoHEAD("This is a sample freeCGI example.");

  //a_Here we start the BODY of our page
  htmlOut.htmlStartBODY();

  //a_This is the content of the page
  htmlOut.htmlDoTagEx("H1", "ALIGN=CENTER", "This is my sample page...");

  //a_You add stuff here...
  
  //a_End our BODY and HTML page
  htmlOut.htmlEndBODY();
  htmlOut.htmlEndHTML();

  return 0x1;                   //a_Return something
}

