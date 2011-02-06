
#include "freeCGI.h"

#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

#ifdef _DEBUG_DUMP_
void ADataItem::dump(void)
{
  //a_Only works if the _DEBUG_DUMP_ flag is set and MIME directive was sent (ie. Content-Type: text/html\n\n)
  cout << endl << "---START: ADataItem::dump()---";
  ABase::dump();
  cout << "  m_padiNext = " << diGetNext() << endl;
  cout << "---END: ADataItem::dump()---" << endl;
}
#endif
