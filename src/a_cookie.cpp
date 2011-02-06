
#include "freeCGI.h"

#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

#ifdef _DEBUG_DUMP_
void ACookie::dump(void)
{
  cout << endl << "---START: ACookie::dump()---";
  
  //a_Do parent dump as well
  AElementPairList::dump();                             

  cout << endl << "---END: ACookie::dump()---" << endl;
}
#endif

//a_This is to be placed inside the mime header!
void ACookie::doOut(AStreamOutput *pasOut) const
{
  //a_Display if there are cookies
  if (pasOut && lGetCount() > 0)
  {
    pasOut->outStringN("Set-Cookie: ");

    doPairs(pasOut, "; ", INT_MAX);      //a_All in one line separated by a semicolon ans space
    pasOut->outStringN("; ");
  }
}

