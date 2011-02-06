#include <freeCGI.h>

int main()
{

  AHTML html;
  ACookie *pcookies = new ACookie[3];

  pcookies[0].plAddItem("item0", "Innoruuk");
  pcookies[1].plAddItem("item1", "Cazic Thule");
  pcookies[2].plAddItem("item2", "Veeshan");

  html.mimeHTML(pcookies, 3);
  html.htmlStartHTML();
  html.htmlStartBODY();
  html << "This is a test of multiple cookies" << endl;
  html.htmlEndBODY();
  html.htmlEndHTML();


  delete []pcookies;
  return 0;
}
