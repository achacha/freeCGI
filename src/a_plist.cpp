
#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

#include "freeCGI.h"

#ifdef _DEBUG_DUMP_
void APairList::dump(void)
{
  //a_Only works if the _DEBUG_DUMP_ flag is set and MIME directive was sent (ie. Content-Type: text/html\n\n)
  cout << endl << "---START: APairList::dump()---";
  AList::dump();

  APairItem *ppiX = CAST(APairItem *, lGetHead());
  while (ppiX)
  {
    ppiX->dump();
    ppiX = CAST(APairItem *, ppiX->diGetNext());
  }
  cout << endl << "------END: APairList::dump()------" << endl;
}
#endif

void APairList::_bCopy(const APairList &plSource)
{
  ABaseElement::_bCopy(plSource);   //a_Parent copy (do not call other list's _bCopy functions)

  APairItem *ppiNew, *ppiX = CAST(APairItem *, lGetHead());
  while (ppiX)
  {
    ppiNew = new APairItem(ppiX);       //a_Copy constructor
    lAdd(ppiNew);
    ppiX = CAST(APairItem *, ppiX->diGetNext());
  }
}

const char *APairList::plGetValueByName(const char *pccName, APairItem *ppiStart, BYTE bPartial, BYTE bAnywhere) const
{
  APairItem *ppiX = plGetItemByName(pccName, ppiStart, bPartial, bAnywhere);
  if (ppiX) return ppiX->piGetValue();

  return NULL;
}

APairItem *APairList::plGetItemByName(const char *pccName, APairItem *ppiStart, BYTE bPartial, BYTE bAnywhere) const
{
  APairItem *ppiX = (ppiStart ? ppiStart : CAST(APairItem *, lGetHead()));

  while (ppiX)
  {
    if (bAnywhere)
    {
      //a_Find anywhere in the NAME field and partial (anywhere implies partial...)
      if (strstr(ppiX->piGetName(), pccName))
      {
        //a_This is only valid while APairItem exists! Pretty safe unless removal of items is done
        return ppiX;
      }
    }
    else
    {
      if (bPartial)
      {
        //a_Partial find of a string at start of the source
        if (!strncmp(ppiX->piGetName(), pccName, strlen(pccName)))
        {
          //a_This is only valid while APairItem exists! Pretty safe unless removal of items is done
          return ppiX;
        }
      }
      else
      {
        //a_Exact match desired in content and size
        if (!strcmp(ppiX->piGetName(), pccName))
        {
          //a_This is only valid while APairItem exists! Pretty safe unless removal of items is done
          return ppiX;
        }
      }
    }

    ppiX = CAST(APairItem *, ppiX->diGetNext());
  }

  return NULL;
}

void APairList::plRemoveItemByName(const char *pccName, int iRemoveAllSameName)
{
  APairItem *ppiX;
  ppiX = plGetItemByName(pccName);
  do
  {
    lRemove(ppiX);
    ppiX = plGetItemByName(pccName);
  } while (ppiX && iRemoveAllSameName);
}

APairItem *APairList::plCreateNewItem(const char *pccName, const char *pccValue)
{
  //a_Just adds, no checking done
  APairItem *ppiNew = NULL;
  if (pccName)
  {
    ppiNew = new APairItem();
    if (ppiNew)
    {
      ppiNew->piSet(pccName, pccValue);
    }
  }
  else
   assert(0x0);             //a_NULL for a name?

  return ppiNew;
}

APairItem *APairList::plAddItem(const char *pccName, const char *pccValue, int iReplace)
{
  //a_Adds with an option to replace the existing, creates new if non-existant
  APairItem *ppiNew = new APairItem;
  if (pccName && ppiNew->piSet(pccName, pccValue))
  {
    //a_Check if exact duplicate exists
    const char *pccS1 = plGetValueByName(ppiNew->piGetName()),
               *pccS2 = ppiNew->piGetValue();

    if ((pccS1 && pccS2) && (0x0 == strcmp(pccS1, pccS2)))
    {
      delete ppiNew;   //a_Already exists
      return NULL;
    }
    else
    {
      if (iReplace)
      {
        //a_Only one NAME = VALUE should exist
        APairItem *ppiFound = plGetItemByName(ppiNew->piGetName());
        if (ppiFound)
        {
          //a_Found that NAME, replace value
          ppiFound->piSetValue(ppiNew->piGetValue());
          delete ppiNew;
        }
        else
        {
          lAdd(ppiNew);            //a_Add this item, was not found
          ppiFound = ppiNew;
        }
        return ppiFound;
      }
      else
      {
        //a_OK to have duplicate NAMEs for different VALUEs
        ADataItem *padiLast = CAST(ADataItem *, lGetLast());
        if (padiLast)
          padiLast->diSetNext(ppiNew);        //a_Append to end of list
        else
          lAdd(ppiNew);                       //a_First item becomes the head
      }
    }
  }
  else
  {
    delete ppiNew;
    return NULL;
  }

  return ppiNew;
}

//a_Output all the pairs
void APairList::doPairs(AStreamOutput *pasOut, char cSeparator, int iItemsPerLine, int iQuoted, int iURLEncode) const
{
  char sTemp[2];
  sTemp[0x0] = cSeparator;
  sTemp[0x1] = '\x0';

  doPairs(pasOut, sTemp, iItemsPerLine, iQuoted, iURLEncode);
}

void APairList::doPairs(AStreamOutput *pasOut, const char *pccSeparator, int iItemsPerLine, int iQuoted, int iURLEncode) const
{
  assert(pasOut);

  if (pasOut)
  {
    APairItem *ppiX = CAST(APairItem *, lGetHead());
    int iPerLine = 0x0;
    int iTempQFlag = 0x0, iTempUFlag = 0x0;

    //a_Iterate the list of items and let them output themselves as NAME=VALUE format
    while (ppiX)
    {
      //a_Enable quotes if needed
      if (iQuoted && !(iTempQFlag = ppiX->piIsFlag(PI_QUOTED)))
        ppiX->piSetFlag(PI_QUOTED);
      else
        ppiX->piClearFlag(PI_QUOTED);

      //a_Enable URL if needed
      if (iURLEncode && !(iTempUFlag = ppiX->piIsFlag(PI_URLENCODED)))
        ppiX->piSetFlag(PI_URLENCODED);
      else
        ppiX->piClearFlag(PI_URLENCODED);

      ppiX->doOut(pasOut);
      if (++iPerLine >= iItemsPerLine)
      {
        iPerLine = 0x0;
        *pasOut << endl;
      }

      //a_Restore the quote flag in the APairItem
      if (iQuoted && !iTempQFlag) ppiX->piClearFlag(PI_QUOTED);

      //a_Retsore the URL Encoding flag
      if (iQuoted && !iTempUFlag) ppiX->piClearFlag(PI_URLENCODED);

      //a_Display separator if there is something else to show and not start of line
      //a_NOTE: for separators like ';' in cookie or '&' in URL, iItemsPerLine should be INT_MAX
      //a_Cookie class will output the trailing ;, which others do not need
      if ((ppiX = CAST(APairItem *, ppiX->diGetNext())) && iPerLine)
      {
        *pasOut << pccSeparator;
      }
    }
  }
}
