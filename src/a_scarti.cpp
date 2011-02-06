
#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

#include "freeCGI.h"

#ifdef _DEBUG_DUMP_
void ASCartItem::dump(void)
{
  //a_Only works if the _DEBUG_DUMP_ flag is set and MIME directive was sent (ie. Content-Type: text/html\n\n)
  cout << endl << "---START: ASCartItem::dump()---";
  APairItem::dump();
  cout << "---END: ASCartItem::dump()---" << endl;
}
#endif

ASCartItem::ASCartItem(ASCartItem *psciSource)
{
  if (psciSource)
    _bCopy(*psciSource);               //a_Copy the form item
  else
  {
    m_iQuantity = 0x00;
  }
}

ASCartItem::~ASCartItem()
{
}

void ASCartItem::_bCopy(const ASCartItem &sciSource)
{
  APairItem::_bCopy(sciSource);         //a_Parent copy

  m_iQuantity = sciSource.sciGetQuantity();
}

void ASCartItem::doOut(AStreamOutput *pasOut) const
{
  APairItem::doOut(pasOut);
}

ASCartItem &ASCartItem::operator +=(int iAddQ)
{ 
  //a_Supports both positive and negative additions
  //a_Doesn't assume iQuantity must be above zero (they don't have to be)
  
  if (m_iQuantity + iAddQ > INT_MAX)
  {
    m_iQuantity = INT_MAX;
  }
  else
  {
    if (m_iQuantity + iAddQ < INT_MIN)
      m_iQuantity = INT_MIN;
    else
      m_iQuantity += iAddQ;
  }
      
  return *this;
}
