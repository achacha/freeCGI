
#include "freeCGI.h"

#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

#ifdef _DEBUG_DUMP_
void AFormList::dump(void)
{
  //a_Only works if the _DEBUG_DUMP_ flag is set and MIME directive was sent (ie. Content-Type: text/html\n\n)
  cout << endl << "---START: AFormList::dump()---";
  APairList::dump();
  cout << endl << "------END: AFormList::dump()------" << endl;
}
#endif

AFormList::AFormList()
{
  m_pccFormData = NULL;
}

AFormList::AFormList(const AFormList& that)
{
  m_pccFormData = aStrDup(that.m_pccFormData);
}

AFormList::~AFormList()
{
  delete []m_pccFormData;
}

void AFormList::_bCopy(const AFormList &flSource)
{
  ABaseElement::_bCopy(flSource);   //a_Parent copy (do not call other list's _bCopy functions)

  AFormItem *pfiNew, *pfiX = CAST(AFormItem *, lGetHead());
  while (pfiX)
  {
    pfiNew = new AFormItem(pfiX);       //a_Copy constructor
    lAdd(pfiNew);
    pfiX = CAST(AFormItem *, pfiX->diGetNext());
  }
}

AFormItem *AFormList::flAddItem(const char *pccItem, int iReplace)
{
  AFormItem *pafiNew = new AFormItem;
  if (pccItem && pafiNew->fiSetAndParse(pccItem))
  {
    //a_Check if exact duplicate exists
    const char *pccS1 = plGetValueByName(pafiNew->piGetName()),
               *pccS2 = pafiNew->piGetValue();

    if ((pccS1 && pccS2) && (0x0 == strcmp(pccS1, pccS2)))
    {
      //a_BOTH NAME and VALUE pair exists
      delete pafiNew;
      return NULL;
    }
    else
    {
      //a_Values are ok and no duplicates, so add to the list
      if (iReplace)
      {
        //a_Only one NAME = VALUE should exist
        AFormItem *pafiFound = CAST(AFormItem *, plGetItemByName(pafiNew->piGetName()));
        if (pafiFound)
        {
          //a_Found that NAME, replace value
          pafiFound->piSetValue(pafiNew->piGetValue());
        }
        delete pafiNew;

        return pafiFound;
      }
      else
      {
        //a_OK to have one NAME = different VALUE
        ADataItem *padiLast = CAST(ADataItem *, lGetLast());
        if (padiLast)
          padiLast->diSetNext(pafiNew);    //a_Append to end of list
        else
          lAdd(pafiNew);                   //a_First item becomes the head
      }
    }
  }
  else
  {
    delete pafiNew;
    return NULL;
  }

  return pafiNew;
}

int AFormList::_flProcessInput(istringstream *pssInput, int iContentLength)
{
  //a_NOTE: will accept istream derrived objects istream_withassign (cin), ifstream, istringstream, etc...
  //a_The stream should terminate in a '\n'(0xD)
  int iCount = 0x0;
  int iLeftOver = iContentLength;
  char *pcBuffer = aStrDup(NULL, FL_MAX_INPUT_BUFFER);

  #ifdef _DEBUG_FULL_
    cout << "<!--AFormList::_formlistProcessInput(istringstream *, " << iContentLength << ")-->\n";
  #endif

  //a_Read in the buffer
  while (!pssInput->eof() && pssInput->peek() != '\x0' && iLeftOver > 0x0)
  {
    pssInput->getline(pcBuffer, FL_MAX_INPUT_BUFFER, '&');   //a_Delimited with '&'

    iLeftOver -= (strlen(pcBuffer) + 0x1);      //a_String length and 1 for '&'

    #ifdef _DEBUG_FULL_
      if(pcBuffer) cout << "<!--FORMITEM found: " << pcBuffer << "-->\n";
    #endif

    if (flAddItem(pcBuffer))
      iCount++;
  }

  #ifdef _DEBUG_FULL_
    cout << "<!--AFormList::_formlistProcessInput(istringstream *,int): iLeftOver=" << iLeftOver << "  iCount=" << iCount << "-->\n";
  #endif

  delete []pcBuffer;

  return iCount;
}

int AFormList::flGenerateList(istream *pisInput, int iContentLength)
{
  #ifdef _DEBUG_FULL_
    cout << "<!--AFormList::flGenerateList(istream *, " << iContentLength << "-->\n";
  #endif

  //a_Accept input stream of known length, generates a strstream and parses it
  //a_cin uses CONTENT_LENGTH to specify length and on Microsoft IIS the EOF is never placed at the end of cin
  if (iContentLength == -1) iContentLength = FL_MAX_INPUT_BUFFER;

  int iRet = 0x0;
  char *pcInput = aStrDup(NULL, iContentLength+1);
  if (pcInput)
  {
    pisInput->read(pcInput, iContentLength);      //a_Delimited with '&', include the NULL
    pcInput[iContentLength] = '\x0';              //a_NULL terminate just in case

    istringstream *pstrInput = new istringstream(pcInput);

    //a_Add item to the raw "list" of items
    assert(m_pccFormData == NULL);               //a_Being called twice?  FORM data was already read in
    delete[] m_pccFormData;
    m_pccFormData = aStrDup(pcInput);

    //a_Generate using istringstream (istream sibling)
    if (pstrInput)
      iRet = _flProcessInput(pstrInput, iContentLength);

    delete pstrInput;
  }

  delete []pcInput;
  return iRet;
}

int AFormList::flGenerateList(const char *pccInput)
{
  #ifdef _DEBUG_FULL_
    cout << "<!--AFormList::flGenerateList(\"" << SAFESTRING(pccInput) << "\")-->\n";
  #endif

  if (!pccInput || *pccInput == '\x0')
    return 0x0;    //a_NULL or empty

  //a_Create a temporary space that is modifiable (chances are the input string is from getenv()
  char *pcInput = aStrDup(pccInput);
  int iRet = 0x0;

  if (pcInput && strlen(pcInput) > 0x1)
  {

    //a_Create a string stream
    istringstream *pstrInput = new istringstream(pcInput);

    //a_Generate using istringstream (istream sibling)
    if (pstrInput)
      iRet = _flProcessInput(pstrInput, strlen(pcInput));

    delete pstrInput;
  }

  delete []pcInput;
  return iRet;
}
