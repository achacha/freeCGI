#include "freeCGI.h"

#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

#ifdef _DEBUG_DUMP_
void ABase::dump(void)
{
  cout << endl << "---START: ABase::dump()---" << endl;
  cout << "this = " << (void *)this;
  cout << endl << "---END: ABase::dump()---" << endl;
}
void ABaseElement::dump(void)
{
  cout << endl << "---START: ABaseElement::dump()---" << endl;
  cout << "this = " << (void *)this;
  cout << endl << "---END: ABaseElement::dump()---" << endl;
}
#endif

