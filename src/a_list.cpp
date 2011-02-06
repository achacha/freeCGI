
#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

#include "freeCGI.h"

#ifdef _DEBUG_DUMP_
void AList::dump(void)
{
  //a_Only works if the _DEBUG_DUMP_ flag is set and MIME directive was sent (ie. Content-Type: text/html\n\n)
  cout << endl << "---START: AList::dump()---";
  ABase::dump();
  cout << "  m_padiHead = " << lGetHead();
  cout << endl << "------END: AList::dump()------" << endl;
}
#endif

void AList::lKill(int iStartIndex)
{
  //a_Remove the associated list of nodes
  ADataItem *padiKiller = lGetAt(iStartIndex - 0x1),  //a_Start at
            *padiNext;

  if (padiKiller)
    padiNext = padiKiller->diGetNext();
  else 
    return;                         //a_Empty list

  padiKiller->diSetNext(NULL);      //a_Terminate the list

  while (padiNext)
  {
    padiKiller = padiNext;
    padiNext   = padiNext->diGetNext();
    delete padiKiller;
  } 

  if (iStartIndex <= 0x0)
  {
    delete m_padiHead;        //a_If index is negative, all other elements have been deleted, so remove the head
    m_padiHead = NULL;        //a_The head is dead :)
  }
}

void AList::lRemove(int iIndex)
{
  //a_Remove the associated list of nodes
  ADataItem *padiBefore = lGetAt(iIndex - 0x1),  //a_Start at
            *padiKill;

  if (padiBefore)
    padiKill = padiBefore->diGetNext();
  else 
    return;                 //a_Empty list

  if (padiKill)
  {
    //a_We have something to kill
    padiBefore->diSetNext(padiKill->diGetNext());        //a_Link over the dead one (soon to be dead that is)
    delete padiKill;
  }
}

//Fix by John Loemban Tobing 17-May-2000: now this function also checks the first element
void AList::lRemove(ADataItem *padiRemove)
{
  if (!m_padiHead)
    return;

  //a_Remove the associated list of nodes
  ADataItem *padiBefore = lGetHead(),
             *padiKill;

  if (padiBefore && padiRemove)
  {
    if (padiBefore == padiRemove)
      padiKill = padiBefore;
    else
      padiKill = padiBefore->diGetNext();
  }
  else
    return;                 //a_Empty list

  while (padiKill)
  {
    //a_We have something to kill
    if (padiKill == padiRemove)
    {
      if (padiKill == padiBefore)
        m_padiHead = m_padiHead->diGetNext();
      else
        padiBefore->diSetNext(padiKill->diGetNext());

      //a_Link over the dead one (soon to be dead that is)
      delete padiKill;
      break;              //a_Done
    }

    padiBefore = padiKill;
    padiKill = padiKill->diGetNext();
  }
}

void AList::lAdd(ADataItem *padiNew)
{
  ADataItem *padiAdd = lGetLast();
  if (padiAdd)
  {
    //a_Add to the end of the list
    padiAdd->diSetNext(padiNew);
  }
  else
  {
    //a_Add at the head
    m_padiHead = padiNew;
  }
}

void AList::lInsertAfter(ADataItem *padiNew, ADataItem *padiAfter)
{
  if (padiAfter)
  {
    //a_Insert after
    padiNew->diSetNext(padiAfter->diGetNext());
    padiNew->diSetNext(padiAfter);
  }
  else
  {
    if (m_padiHead)
    {
      //a_Insert at the head
      padiNew->diSetNext(m_padiHead);
    }
    m_padiHead = padiNew;
  }
}

int AList::lGetCount(void) const
{
  int iRet = 0x0;
  ADataItem *padiX = lGetHead();

  while (padiX)
  {
    iRet++;
    padiX = padiX->diGetNext();
  }

  return iRet;
}

ADataItem *AList::lGetAt(int iIndex) const
{
  ADataItem *padiX = lGetHead();
  int iAt = 0x0;

  //a_If the index is too large NULL is returned, if negative then HEAD is returned
  while (iAt < iIndex && padiX)
  {
    iAt++;
    padiX = padiX->diGetNext();
  }

  return padiX;
}

ADataItem *AList::lGetLast(void) const
{
  ADataItem *padiRet = NULL, *padiX = lGetHead();

  while (padiX)
  {
   padiRet = padiX;
   padiX   = padiX->diGetNext();
  }
  return padiRet;
}
