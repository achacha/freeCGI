
#include "freeCGI.h"

#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

#ifdef _DEBUG_DUMP_
void AFormItem::dump(void)
{
  //a_Only works if the _DEBUG_DUMP_ flag is set and MIME directive was sent (ie. Content-Type: text/html\n\n)
  cout << endl << "---START: AFormItem::dump()---";
  APairItem::dump();
  cout << "---END: AFormItem::dump()---" << endl;
}
#endif

void AFormItem::_bCopy(const AFormItem &fiSource)
{
  //a_Parents' copy
  APairItem::_bCopy(fiSource);
} 

//a_Given in a format common to CGI forms eg. %7ENAME=This is my name (& have been removed!)
int AFormItem::fiSetAndParse(const char *pccItem)
{
  if (!pccItem) return 0x0;                      //a_Can't be NULL

  const char *pccX = pccItem;
  char *pcTemp = aStrDup(pccItem),      //a_Will be at least same size (but may be smaller due to %XXs)
       *pcName = pcTemp, *pcValue = NULL, *pcPos = pcTemp;
  
  int iStartFound =0x0;

  //a_String passed here MUST be NULL terminated!
  while (*pccX)
  {
    switch(*pccX)
    {
      case '+' : //a_Replace with spaces
        *(pcPos++) = ' ';
        pccX++;
      break;

      case '%' : //a_Escape sequence to follow
      {
        *(pcPos++) = g_aCrypto.convertoHEXtoBYTE(*(pccX + 1), *(pccX + 2));  //a_Convert to integer and cast to char (2 digit hex entry) 

        pccX += 3;                          //a_Next entry
      }
      break;

      case '=' : //a_Separator found
      {
        if (!iStartFound)
        {
          *(pcPos++) = 0x0;                  //a_NULL terminate
          pcValue = pcPos;                   //a_Start of Value :)
          iStartFound = 0x1;                 //a_Now it is found
          pccX++;                             //a_Go to start of Value portion, ignore this '='
        }
        else
          *(pcPos++) = *(pccX++);
      }
      break;
    
      default :
        *(pcPos++) = *(pccX++);
    } 
  }

  *pcPos = 0x0;

   int iRet = piSet(pcName, pcValue);
   
   delete []pcTemp;

   return iRet;
}

