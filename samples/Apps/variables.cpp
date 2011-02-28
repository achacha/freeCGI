/*
  This CGI is meant to provide a simple "Hello World" app that can be used to create other CGIs
*/

#define _DEBUG_DUMP_
#include <freeCGI.h>

#define NAME_VARNAME "!EnvName"

int main()
{
  //a_Create an instance of the CGI output object
  ACGI acgi;

  //a_Output the MIME type for the document (HTML = text/html)
  acgi.mimeHTML();

  //a_Start HTML documeent
  acgi.htmlStartHTML();
  acgi.htmlDoHEAD("CGI Variable Tool");
  acgi.htmlStartBODY();

  //a_Gather Form/QueryString variables into the ACGI object
  acgi.cgiGetFormItems();

  //a_Get the HTTP cookie variables
  acgi.cgiGetHTTPCookie();

  //a_Display Query String
  acgi.htmlDoTag("h2", "QueryString");
  acgi << acgi.cgiGetQueryString() << "<br/>\r\n";

  //a_Display the variable query form
#ifndef NDEBUG
  acgi.htmlDoTag("h2", "Look up Environment Variable");
  acgi.htmlDoTag("b", "NOTE:");
  acgi.htmlDoTag("small", "HTTP Request Header variables all start with <b>HTTP_</b>, dashes are replaced with underscores (e.g. <b>User-Agent</b> becomes <b>HTTP_USER_AGENT</b>)");
  acgi.htmlTag("br");
  acgi.htmlTag("br");
  acgi.htmlStartTag("form", "method=\"post\"");
    acgi << "Variable Name: ";
    acgi.cgiDoFORMInput(FORMINPUT_TEXT, NAME_VARNAME, NULL, NULL, "size=\"30\"");
    acgi.htmlTag("input", "type=\"submit\"");
  acgi.htmlEndTag("form");
#endif

  const char *pccValue = acgi.plGetValueByName(NAME_VARNAME);
  if (pccValue)
  {
    //a_Display the result in a table
    acgi.htmlDoTag("h2", "Environment Variable");
      acgi.htmlStartTag("table", "border=\"2\"");
        acgi.htmlStartTag("tr");
          acgi.htmlDoTagEx("th", "align=\"right\"", pccValue);
          acgi.htmlDoTagEx("td", "bgcolor=\"#404040\" style=\"color: rgb(248,248,220)\"", acgi.cgiGetSafeEnv(pccValue));
        acgi.htmlEndTag("tr");
      acgi.htmlEndTag("table");
  }

  //a_Dump the environment
  acgi.cgiEnvironmentDump(0x1);

  acgi.htmlEndBODY();
  acgi.htmlEndHTML();

  return 0x0;
}
