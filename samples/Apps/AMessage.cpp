//
// AMessage see AC_TITLE below for version #
//

//a_Comment out to prevent debbugging dumps (ie. notifications upon SNAFU)
//#define _DEBUG_DUMP_              //a_Enable debugging
//#define _DEBUG_FULL_              //a_Intense debugging dump
//#define _SHOW_HELP_               //a_Online help HREF
//#define _TEST_                    //a_Enable if testing; ie. FORM items posted via string

#include "freeCGI.h"
#include <sstream>

#define AC_TITLE     "AMessage: Messages for the World Wide Web: "
#define AC_HTMLTITLE "<H3 ALIGN=CENTER><B>AMessage</B>: Messages for the World Wide Web <I>v1.0</I></H3>"

#define AC_FILENAME   "amessage.dat"
#define AC_MAXOPENTRIES 0xA          //a_Try 10 times
#define AC_MAXDATALINE 4096          //a_Max linesize during reading of data

#define AC_BACKGROUND "http://www.achacha.org/images/amessage.jpg"   //a_Default background bitmap
#define AC_COLOR_BACK 0x00000000              //a_Background color
#define AC_COLOR_TEXT 0x00FEFEFE              //a_Text color

//Format per message
//{STARTTOKEN}
//{HTML block}
//{ENDTOKEN}
#define AC_STARTTOKEN "<---START--->"
#define AC_STARTTOKEN_LEN 13
#define AC_ENDTOKEN   "<----END---->"
#define AC_ENDTOKEN_LEN 13

#define NAME_NAME       "!NAME"               //a_User's name/alias
#define NAME_MESSAGE    "!MESSAGE"            //a_The message to be posted
#define NAME_EMAIL      "!EMAIL"              //a_User's e-mail
#define NAME_IMAGE      "!IMAGE"              //s_User's image URL
#define NAME_HOMEPAGE   "!HOMEPAGE"           //a_User's homepage URL
#define NAME_TITLE      "!TITLE"              //a_HTML title override
#define NAME_BACKGROUND "!BG"                 //a_The background image for the page

#define NAME_DOFORM     "!DOFORM"             //a_If set to '0' then no form is generated
#define NAME_OUTFRAME   "!OUTFRAME"           //a_If set output is sent to the frame and only form is generated
#define NAME_FILENAME   "!FILENAME"           //a_Data filename to use, adds .dat

#define AC_MAXBUFFER  0x6000                //a_Maximum buffer size for the HTML block
#define AC_MAXENTRIES 0x20                  //a_# of items saved

#define OUTPUT_STACK  0x1                   //a_When 0x0, newest message is at the bottom with the form

//a_Pre-declare functions
int getData(fstream &fsIn, char *sBuffer, int iMaxLength);
void doForm(ACGI &acgiOut);

#ifdef _DEBUG_DUMP_
void doDump(ACGI &acgiForm);
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
  ACGI acgiOut;

  //a_Output initial info
  acgiOut.mimeHTML();   //a_Declare the MIME directive for HTML
  acgiOut.htmlStartHTML();

  const char *pccValue;

  //a_See if data was submitted by a form
  #ifdef _TEST_
    //a_This is a debugging way of submitting a string instead of relying of the web server
    //a_This is used for testing only
    char *pcInput = new char[512];
    strcpy(pcInput, "%21NAME=User&%21FILENAME=mdata&%21MESSAGE=Hello");
    int iFormItems = acgiOut.cgiGetFormItems(pcInput);
  #else
    int iFormItems = acgiOut.cgiGetFormItems();
    iFormItems += acgiOut.cgiGetQueryStringItems();
  #endif

  //a_The title of the page
  if ((pccValue = acgiOut.plGetValueByName(NAME_TITLE)))
    acgiOut.htmlDoHEAD(AC_TITLE, pccValue);
  else
  {
    if ((pccValue = acgiOut.plGetValueByName(NAME_FILENAME)))
      acgiOut.htmlDoHEAD(AC_TITLE, pccValue);
    else
      acgiOut.htmlDoHEAD(AC_TITLE, " ");
  }

  //a_Output the list as HTML
  if ((pccValue = acgiOut.plGetValueByName(NAME_OUTFRAME)))
  {
      //a_Data entry frame
    #ifdef _DEBUG_DUMP_
      acgiOut << endl << "<!--Input frame; !OUTFRAME =" << pccValue << "-->" << endl;
    #endif
      if ((pccValue = acgiOut.plGetValueByName(NAME_BACKGROUND)))
        acgiOut.htmlStartBODY(AC_COLOR_BACK, AC_COLOR_TEXT, RGB_LYELLOW, RGB_MYELLOW, RGB_WHITE, pccValue);
      else
        acgiOut.htmlStartBODY(AC_COLOR_BACK, AC_COLOR_TEXT, RGB_LYELLOW, RGB_MYELLOW, RGB_WHITE, AC_BACKGROUND);

    acgiOut << endl << AC_HTMLTITLE << endl;            //a_Heading
  }
  else
  {
    //a_Output
    if ((pccValue = acgiOut.plGetValueByName(NAME_DOFORM)) && pccValue[0] == '0')
    {
      //a_Output to a frame
      #ifdef _DEBUG_DUMP_
        acgiOut << endl << "<!--Output frame; No title-->" << endl;
      #endif
      acgiOut << endl;
      if ((pccValue = acgiOut.plGetValueByName(NAME_BACKGROUND)))
        acgiOut.htmlStartBODY(AC_COLOR_BACK, AC_COLOR_TEXT, RGB_LYELLOW, RGB_MYELLOW, RGB_WHITE, pccValue);
      else
        acgiOut.htmlStartBODY(AC_COLOR_BACK, AC_COLOR_TEXT, RGB_LYELLOW, RGB_MYELLOW, RGB_WHITE, AC_BACKGROUND);

      if ((pccValue = acgiOut.plGetValueByName(NAME_TITLE)))
      {
        acgiOut.htmlStartTag("CENTER");
        acgiOut.htmlDoTag("H2", pccValue);
        acgiOut.htmlEndTag("CENTER");
      }
    }
    else
    {
      //a_Standalone, no frames
      #ifdef _DEBUG_DUMP_
        acgiOut << endl << "<!--No frames; Doing title; !OUTFRAME = (nil)-->" << endl;
      #endif


      if ((pccValue = acgiOut.plGetValueByName(NAME_BACKGROUND)))
        acgiOut.htmlStartBODY(AC_COLOR_BACK, AC_COLOR_TEXT, RGB_LYELLOW, RGB_MYELLOW, RGB_WHITE, pccValue);
      else
        acgiOut.htmlStartBODY(AC_COLOR_BACK, AC_COLOR_TEXT, RGB_LYELLOW, RGB_MYELLOW, RGB_WHITE, AC_BACKGROUND);

      //a_Is title overridden?
      if ((pccValue = acgiOut.plGetValueByName(NAME_TITLE)))
      {
        acgiOut.htmlStartTag("CENTER");
        acgiOut.htmlDoTag("H2", pccValue);
        acgiOut.htmlEndTag("CENTER");
      }
      else
        acgiOut << AC_HTMLTITLE << endl;            //a_Heading
    }
  }

  int iAddNew = (iFormItems > 0x0 ? 0x1 : 0x0);
  pccValue = acgiOut.plGetValueByName(NAME_MESSAGE);
  if (!acgiOut.plGetValueByName(NAME_NAME) ||
      !*acgiOut.plGetValueByName(NAME_NAME) ||
      !pccValue ||
       acgiOut.cgiIsNotValidHTMLLine(pccValue))
  {
    iAddNew = 0x0;                //a_Omitted required item, so no post, just update
    #ifdef _DEBUG_DUMP_
      acgiOut << endl << "<!--Invalid: Name || Message -->" << endl;
    #endif
  }

  //a_Open the data file
  fstream fsData;
  char *sBuffer = new char[AC_MAXBUFFER];
  sBuffer[0] = 0x0;

  if ((pccValue = acgiOut.plGetValueByName(NAME_FILENAME)))
  {
    //a_Filename found
    #ifdef _DEBUG_DUMP_
      acgiOut << endl << "<!--Filename to use: " << pccValue << " -->" << endl;
    #endif
    strcpy(sBuffer, pccValue);    //a_Filename
    strcat(sBuffer, ".dat");      //a_Extension
  }
  else
  {
    //a_Try to use the script name as a save filename
    if ((pccValue = acgiOut.cgiGetScriptName()))
    {

      #ifdef _DEBUG_DUMP_
        acgiOut << endl << "<!--SCRIPTNAME = ";
        acgiOut << pccValue;
        acgiOut << "-->" << endl;
      #endif

      //a_First remove the path (current is assumed)
      char *pcExt;
      const char *pccFind;

      pccFind = strrchr(pccValue, '/');            //a_Find extension

      strcpy(sBuffer, (pccFind ? pccFind + 1 : AC_FILENAME));

      //a_Find extension
      pcExt = strrchr(sBuffer, '.');
      if (pcExt)
        strcpy(pcExt + 0x1, "dat");                   //a_Replace extension
      else
        strcat(sBuffer, ".dat");                      //a_No extension found

      #ifdef _DEBUG_DUMP_
        acgiOut << endl << "<!--Filename to read = ";
        acgiOut << sBuffer;
        acgiOut << "-->" << endl;
      #endif
    }
    else
    {
      acgiOut.htmlDoTag("I", "Default filename");
      strcpy(sBuffer, AC_FILENAME);

      #ifdef _DEBUG_DUMP_
        acgiOut << endl << "<!--DEFAULT ACTION: Filename opened for read = ";
        acgiOut << AC_FILENAME;
        acgiOut << "-->" << endl;
      #endif

    }
  }
  //a_Save it for later use
  char *pcDatafile = aStrDup(sBuffer);

  char *ppcData[AC_MAXENTRIES];
  int iEntryNumber;

  for (iEntryNumber = AC_MAXENTRIES; iEntryNumber > 0x0; iEntryNumber--)
    ppcData[iEntryNumber - 0x1] = NULL;

  fsData.open(pcDatafile, ios::in);        //a_Use the default

  //a_Find AC_STARTTOKEN
  iEntryNumber = iAddNew;              //a_If form was submitted then 0x0th one is the new one
#if defined(__BORLANDC__) || defined(__unix)
  if ((fsData.rdbuf())->is_open())
#else
  if (fsData.is_open())
#endif
  {
    #ifdef _DEBUG_DUMP_
      acgiOut << endl << "<!--Filename opened for read = ";
      acgiOut << pcDatafile;
      acgiOut << "-->" << endl;
    #endif

    while (getData(fsData, sBuffer, AC_MAXBUFFER) && iEntryNumber < AC_MAXENTRIES)
    {
      //a_Data to follow is an HTML Block
      #ifdef _DEBUG_DUMP_
        acgiOut << endl << "<!--Read item #" << iEntryNumber << "-->" << endl;
      #endif

      ppcData[iEntryNumber] = aStrDup(sBuffer);
      iEntryNumber++;
    }
  }
  else
  {
    #ifdef _DEBUG_DUMP_
      acgiOut << endl << "<!--Filename NOT opened for read = ";
      acgiOut << pcDatafile;
      acgiOut << "-->" << endl;
    #endif
  }

  //a_Add the new entry to the list, need to generate the new entry
  if (iAddNew)
  {
    std::stringstream *pstrNew = new std::stringstream(sBuffer, ios::out);
    if (pstrNew)
    {
      AHTML htmlNew;
      htmlNew.setOStream(pstrNew);           //a_Output stream is strstream

      htmlNew << "<P>";
      //a_If E-Mail was entered use it
      pccValue = acgiOut.plGetValueByName(NAME_EMAIL);
      if (pccValue && *pccValue && acgiOut.cgiIsValidEMail(pccValue))
      {
        //a_Embed the picture
        const char *pccValue2 = acgiOut.plGetValueByName(NAME_IMAGE);
        if (acgiOut.cgiIsValidURL(pccValue2))
        {
          htmlNew << "<IMG ALIGN=LEFT SRC=\"";
          htmlNew << pccValue2;
          htmlNew << "\">";
        }
        else
        {
          #ifdef _DEBUG_DUMP_
            acgiOut << endl << "<!--main:1: Invalid HREF: ";
            acgiOut << pccValue;
            acgiOut << "-->" << endl;
          #endif
        }

        //a_Email exists
        htmlNew << "<A HREF=\"mailto:";
        htmlNew << pccValue;
        htmlNew << "\">" << endl;

        htmlNew.htmlStartTag("BIG");
        pccValue2 = acgiOut.plGetValueByName(NAME_NAME);
        if (!acgiOut.cgiIsNotValidHTMLLine(pccValue2))
           htmlNew.htmlDoTag("B", pccValue2);
        else
          htmlNew.htmlDoTag("B", "Anonymous");
        htmlNew.htmlEndTag("BIG");
        htmlNew << endl << "&lt;" << pccValue << "&gt;";  //a_Display e-mail
        htmlNew.htmlEndTag("A");

		    //a_Time and date stamp
	    	htmlNew.htmlStartTag("I");
        htmlNew << "   on ";
        htmlNew.htmlDateTime();
        htmlNew.htmlEndTag("I");

        //a_Do the homepage is any
        pccValue = acgiOut.plGetValueByName(NAME_HOMEPAGE);
        if (acgiOut.cgiIsValidURL(pccValue))
        {
          //a_Dislpay the home address
          htmlNew << "<BR>";
          htmlNew << "<A HREF=\"" << pccValue << "\" TARGET=_top>";
          htmlNew.htmlDoTag("I", pccValue);
          htmlNew.htmlEndTag("A");
        }
      }
      else
      {
        //a_Just the name, no EMail
        //a_Embed the picture
        pccValue = acgiOut.plGetValueByName(NAME_IMAGE);
        if (acgiOut.cgiIsValidURL(pccValue))
        {
          htmlNew << "<IMG ALIGN=LEFT SRC=\"";
          htmlNew << pccValue;
          htmlNew << "\">" << endl;
        }
        else
        {
          #ifdef _DEBUG_DUMP_
           acgiOut << endl << "<!--main:2: Invalid HREF: ";
           acgiOut << pccValue;
           acgiOut << "-->" << endl;
          #endif
        }

        htmlNew.htmlStartTag("BIG");
        pccValue = acgiOut.plGetValueByName(NAME_NAME);
        if (!acgiOut.cgiIsNotValidHTMLLine(pccValue))
          htmlNew.htmlDoTag("B", pccValue);
        else
          htmlNew.htmlDoTag("B", "Anonymous");
        htmlNew.htmlEndTag("BIG");

        //a_Show time and date of the post
        htmlNew.htmlStartTag("I");
        htmlNew << "   on ";
        htmlNew.htmlDateTime();
        htmlNew.htmlEndTag("I");

        //a_Do the homepage is any
        pccValue = acgiOut.plGetValueByName(NAME_HOMEPAGE);
        if (acgiOut.cgiIsValidURL(pccValue))
        {
          //a_Dislpay the home address
          htmlNew << "<BR>";
          htmlNew << "<A HREF=\"" << pccValue << "\">";
          htmlNew.htmlDoTag("I", pccValue);
          htmlNew.htmlEndTag("A");
        }
      }

      htmlNew << "<BR CLEAR=ALL>" << endl << "<P>";

      //a_Message should have been checked for validity higher above
      pccValue = acgiOut.plGetValueByName(NAME_MESSAGE);
      char *pcMessage = aStrDup(pccValue);
      if (pcMessage)
      {
        //a_Format and output the message block (ie. text to HTML) and such
        //a_This is to be done only once upon adding...
        acgiOut.cgiValidateHTMLLine(pcMessage);
        char *pcS = pcMessage, *pcE;
        do
        {
          if ((pcE = strchr(pcS, '\n')))
          {
            *pcE = 0x0;
            if (*(pcE - 0x1) == '\r') *(pcE - 0x1) = 0x0;
            if (*(pcE + 0x1) == '\r') *(++pcE) = 0x0;
          }
          else
          {
            pcE = strchr(pcS, '\r');    //a_Maybe only '\r' is used ?!?
          }

          if (pcS)
            htmlNew << pcS << "<BR>" << endl;

          if (pcE)
            pcS = pcE + 0x1;

        } while (pcE);
      }
      else
      {
        //a_Just dump it, could not allocate the buffer to format the message
        htmlNew << pccValue;
      }
      htmlNew << "<BR CLEAR=ALL></A></FONT></B></CENTER>" << endl;

      //a_Save IP in a comment
      htmlNew << "<!--";
      htmlNew << acgiOut.cgiGetRemoteAddress();
      htmlNew << "\"--><HR SIZE=2>" << ends;
    }
    else
    {
      strcpy(sBuffer, "<P>");
      strcat(sBuffer, acgiOut.plGetValueByName(NAME_NAME));
      strcat(sBuffer, "<BR CLEAR=ALL>");
      strcat(sBuffer, acgiOut.plGetValueByName(NAME_MESSAGE));
      strcat(sBuffer, "<BR><HR SIZE=2>\r\n");
    }

    delete pstrNew;

    sBuffer[AC_MAXBUFFER - 0x1] = 0x0;  //a_Safety first, the string condom ;)
    ppcData[0x0] = sBuffer;
  }

  //a_Save the new file, then do the HTML output
#if defined(__BORLANDC__) || defined(__unix)
  if ((fsData.rdbuf())->is_open())
#else
  if (fsData.is_open())
#endif
    fsData.close();

  if (acgiOut.plGetValueByName(NAME_MESSAGE) && iAddNew)
  {
    //a_Write is not an update
    int iTries = 0x0;
#if defined(__BORLANDC__) || defined(__unix)
    if (!(fsData.rdbuf())->is_open())
#else
    if (!fsData.is_open())
#endif
    {
      #ifdef _DEBUG_DUMP_
        acgiOut << endl << "<!--Attempting to open the file-->";
      #endif
      while (iTries++ < AC_MAXOPENTRIES)
      {
        if (pcDatafile)
        {
          #ifdef _DEBUG_DUMP_
            acgiOut << endl << "<!--Opening " << pcDatafile << " for writing-->";
          #endif

          fsData.open(pcDatafile, ios::out);
        }

#if defined(__BORLANDC__) || defined(__unix)
        if (!(fsData.rdbuf())->is_open())
#else
        if (!fsData.is_open())
#endif
        {
          //a_Delay
          time_t ttStart, ttEnd;
          time(&ttStart);
          time(&ttEnd);
          #ifdef _DEBUG_DUMP_
            acgiOut << endl << "<!--Delaying and waiting for unlock-->";
          #endif

          while (difftime(ttEnd, ttStart) < 0x1) time(&ttEnd);   //a_Wait
        }
        else
          iTries = AC_MAXOPENTRIES;
      }
    }
    else
    {
      #ifdef _DEBUG_DUMP_
        acgiOut << endl << "<!--Filename NOT opened for write = ";
        acgiOut << pcDatafile;
        acgiOut << "-->" << endl;
      #endif

      return 0x3;
    }

    fsData.seekp(0x0, ios::beg);    //a_Go to the beginning
    for (iEntryNumber = 0x0; iEntryNumber < AC_MAXENTRIES; iEntryNumber++)
    {
      if (ppcData[iEntryNumber])
      {
        #ifdef _DEBUG_DUMP_
          acgiOut << "<!--Wrote item #" << iEntryNumber << "-->" << endl;
        #endif

        fsData << AC_STARTTOKEN << endl;
        fsData << ppcData[iEntryNumber] << endl;
        fsData << AC_ENDTOKEN << endl;
      }
      else
      {
        #ifdef _DEBUG_DUMP_
          acgiOut << "<!--Skipped item #" << iEntryNumber << "-->" << endl;
        #endif
      }
    }
    fsData.flush();
    fsData.close();
  }
  else
  {
    #ifdef _DEBUG_DUMP_
      acgiOut << endl << "<!--No message specified, skipping the write stage-->";
    #endif
  }

  #ifdef OUTPUT_STACK
    //a_Stack like behavior, form at the top (if any)
    if ((pccValue = acgiOut.plGetValueByName(NAME_DOFORM)))
    {
      //a_Only setting it to 0 will disable forms
      if (pccValue[0] != '0')
        doForm(acgiOut);
    }
    else
    {
      //a_DoForm not specified, show the form :)
      doForm(acgiOut);
    }
  #endif

  //a_If an output frame is not specified, dump output
  if (!(pccValue = acgiOut.plGetValueByName(NAME_OUTFRAME)))
  {
    #ifdef _DEBUG_DUMP_
      acgiOut << endl << "<!--!OUTFRAME not specified-->" << endl;
    #endif

		#ifdef OUTPUT_STACK
		  //a_Stack like behavior, latest message at the top
		  for (iEntryNumber = 0x0; iEntryNumber < AC_MAXENTRIES; iEntryNumber++)
		#else
		  //a_Bulletin board behavior, latest appended at the bottom
		  for (iEntryNumber = AC_MAXENTRIES - 0x1; iEntryNumber >= 0x0; iEntryNumber--)
		#endif
		{
		  #ifdef _DEBUG_DUMP_
		  	acgiOut << endl << "<!--Output line #" << iEntryNumber << "-->";
		  #endif
		  if (ppcData[iEntryNumber])
		  {
        acgiOut << ppcData[iEntryNumber];
        acgiOut << endl;
		  }
		  delete []ppcData[iEntryNumber];               //a_Cleanup while at it (including sBuffer!)
		}
  }
  else
  {
    #ifdef _DEBUG_DUMP_
      acgiOut << endl << "<!--!OUTFRAME specified, skipping output-->" << endl;
    #endif
  }

  pccValue = acgiOut.plGetValueByName(NAME_DOFORM);
  #ifndef OUTPUT_STACK
    //a_Newest appended to the end and the form is at the bottom (if any)
    if (pccValue)
    {
      //a_Only setting it to 0 will disable forms
      if (pccValue[0] != '0')
        doForm(acgiOut);
    }
    else
    {
      //a_no !DOFORM variable, so show the form
      doForm(acgiOut);
    }
  #endif

  #ifdef _DEBUG_DUMP_
    acgiOut << endl << "<!--...the end...-->" << endl;
    doDump(acgiOut);
    acgiOut.doOut(&acgiOut);
  #endif

  acgiOut << "<BR>" << endl;


  //a_Close the HTML code
  acgiOut.htmlEndBODY();
  acgiOut.htmlEndHTML();

  delete []pcDatafile;

#ifdef _TEST_
  delete []pcInput;
#endif

  return 0x0;
}

//a_Exists when debugging only
#ifdef _DEBUG_DUMP_
void doDump(ACGI &acgiForm)
{
  acgiForm << "<!--" << endl;
  acgiForm.cgiEnvironmentDump();
  acgiForm << "-->" << endl;
}
#endif

int getData(fstream &fsIn, char *sBuffer, int iMaxLength)
{
  char sTemp[AC_MAXDATALINE + 0x1];
  int iLength = iMaxLength, iReadLength;
  sBuffer[0] = 0x0;

  while (!fsIn.eof())
  {
    fsIn.getline(sTemp, AC_MAXDATALINE);
    if (strstr(sTemp, AC_STARTTOKEN) == sTemp)
    {
      while (!fsIn.eof())
      {
        //a_Read in data until the ENDTOKEN is found
        fsIn.getline(sTemp, AC_MAXDATALINE);
        if (strstr(sTemp, AC_ENDTOKEN) == sTemp)
          return 0x1;                           //a_Found

        iReadLength = strlen(sTemp);
        iLength -= iReadLength;
        if (iLength >= 0x0)
        {
          //a_Room left in the buffer
          if (iReadLength > 0x0)
          {
            strcat(sBuffer, sTemp);
            strcat(sBuffer, "\n");   //a_Only a CR here...
          }
        }
        else
        {
          strncat(sBuffer, sTemp, strlen(sTemp) - iLength);
          sBuffer[iMaxLength - 2] = '>';   //a_Delimit a truncation due to buffer "smallness" :)
          sBuffer[iMaxLength - 1] = 0x0;   //a_NULL terminate

          //a_Now find the ENDTOKEN and read it in
          while (!fsIn.eof())
          {
            //a_Read in data until the ENDTOKEN is found
            fsIn.getline(sTemp, AC_MAXDATALINE);
            if (fsIn.eof() || strstr(sTemp, AC_ENDTOKEN) == sTemp)
              return 0x1;                           //a_Found
          }
        }
      }
    }
  }

  return 0x0;                               //a_Not found
}

void doForm(ACGI &acgiOut)
{
  //a_Generate the form for posting
  const char *pccValue;
  acgiOut << endl << "<FORM METHOD=\"POST\" ACTION=\"";

  //a_Redirect output to self with parameters
  acgiOut << acgiOut.cgiGetScriptName();
  if ((pccValue = acgiOut.plGetValueByName(NAME_FILENAME)))
  {
    acgiOut << "?" << NAME_FILENAME;    //a_Filename used
    acgiOut << "=" << pccValue;
  }

  //a_See if there is a title
  if (acgiOut.plGetValueByName(NAME_TITLE))
  {
    if (!pccValue) acgiOut << "?";     //a_First get item
    else           acgiOut << "&";     //a_Already have a GET item

    //a_Title used
    acgiOut << NAME_TITLE << "=" << acgiOut.plGetValueByName(NAME_TITLE);
  }

  //a_See if there is a title
  if (acgiOut.plGetValueByName(NAME_BACKGROUND))
  {
    if (!pccValue) acgiOut << "?";     //a_First get item
    else           acgiOut << "&";     //a_Already have a GET item

    //a_Title used
    acgiOut << NAME_BACKGROUND << "=" << acgiOut.plGetValueByName(NAME_BACKGROUND);
  }

  //a_See if there is an output frame
  if (acgiOut.plGetValueByName(NAME_OUTFRAME))
  {
    if (!pccValue) acgiOut << "?";     //a_First get item
    else           acgiOut << "&";     //a_Already have a GET item

    //a_Don't need a form in a frame
    acgiOut << NAME_DOFORM << "=0";
    //a_Redirect output to a frame
    acgiOut << "\" TARGET=\"" << acgiOut.plGetValueByName(NAME_OUTFRAME);
  }

  acgiOut << "\">" << endl;
  acgiOut << "<TT><B>Name: </B></TT>\n<INPUT TYPE=TEXT NAME=\"";
  acgiOut << NAME_NAME;

  if ((pccValue = acgiOut.plGetValueByName(NAME_NAME)))
  {
    //a_Fill in the name already used
    acgiOut << "\" VALUE=\"" << acgiOut.plGetValueByName(NAME_NAME);
  }

  acgiOut << "\" SIZE=24>   Time is ";
  acgiOut.htmlStartTag("I");
  acgiOut.htmlDateTime();
  acgiOut.htmlEndTag("I");

  acgiOut << "<BR>" << endl << "<TT><B>Text: </B></TT>\n<TEXTAREA NAME=\"";
  acgiOut << NAME_MESSAGE;
  acgiOut << "\" COLS=60 ROWS=3></TEXTAREA><BR>" << endl;
  acgiOut << "<TT>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<INPUT TYPE=SUBMIT VALUE=\"ACTION\">\n&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<INPUT TYPE=RESET>\n</TT>";

  #ifdef _SHOW_HELP_
    acgiOut << " &nbsp; &nbsp;  <a href=\"/adc/common/help.html\"";
    if (pccValue)
      acgiOut << " target=\"_top\"";  //a_Frame redirection
    acgiOut << "><I>Online Help</I></A><BR>" << endl;
  #endif

  acgiOut << "<P>Optional Info<BR><TT>" << endl;
  acgiOut << "<table><tr><th>E-Mail</th><td><INPUT TYPE=TEXT NAME=\"";
  acgiOut << NAME_EMAIL;
  pccValue = acgiOut.plGetValueByName(NAME_EMAIL);
  if (pccValue && acgiOut.cgiIsValidEMail(pccValue))
  {
    //a_Fill in the e-mail address
    acgiOut << "\" VALUE=\"" << pccValue;
  }
  acgiOut << "\" SIZE=40></td></tr>" << endl;

  acgiOut << "<tr><th>Image URL</th><td><INPUT TYPE=TEXT NAME=\"";
  acgiOut << NAME_IMAGE;

  pccValue = acgiOut.plGetValueByName(NAME_IMAGE);

  //a_Fill in the image URL
  if (acgiOut.cgiIsValidURL(pccValue))
  {
    acgiOut << "\" VALUE=\"" << pccValue;;
  }
  acgiOut << "\" SIZE=40></td></tr>" << endl;

  acgiOut << "<tr><th>Homepage URL</th><td><INPUT TYPE=TEXT NAME=\"";
  acgiOut << NAME_HOMEPAGE;

  pccValue = acgiOut.plGetValueByName(NAME_HOMEPAGE);

  //a_Fill in the homepage URL
  if (acgiOut.cgiIsValidURL(pccValue))
  {
    acgiOut << "\" VALUE=\"" << pccValue;;
  }

  acgiOut << "\" SIZE=40></td></tr></table></form><hr size=6/>" << endl;

  #ifdef _DEBUG_DUMP_
    //a_Only during debugging...
    pccValue = acgiOut.plGetValueByName(NAME_EMAIL);
    if (acgiOut.cgiIsValidEMail(pccValue))
    {
      acgiOut << endl << "<!--doForm: Invalid !EMAIL-->" << endl;
    }

    pccValue = acgiOut.plGetValueByName(NAME_IMAGE);
    if (acgiOut.cgiIsValidURL(pccValue))
    {
      acgiOut << endl << "<!--doForm: Invalid HREF: !IMAGE-->" << endl;
    }

    pccValue = acgiOut.plGetValueByName(NAME_HOMEPAGE);
    if (acgiOut.cgiIsValidURL(pccValue))
    {
      acgiOut << endl << "<!--doForm: Invalid HREF: !HOMEPAGE-->" << endl;
    }
  #endif
}


