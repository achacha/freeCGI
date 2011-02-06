
#include "freeCGI.h"

#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

#ifdef _DEBUG_DUMP_
void AElement::dump(void)
{
  cout << endl << "---START: AElement::dump()---";

  AElementPairList::dump();              //a_Do parent dump as well

  //a_AElement specific
  cout << "m_pcTag=" << m_pcTag << endl;

  cout << "---END: AElement::dump()---" << endl;
}
#endif

AElement::AElement(AElement *paeSource)
{
  m_pcTag = NULL;             //a_NULL this to make sure _bCopy doesn't delete[] invalid pointer

  if (paeSource)
  {
    //a_Copy constructor
    _bCopy(*paeSource);
  }
  else
  {
    //a_Default constructor
  }
}

AElement::~AElement(void)
{
  delete []m_pcTag;
}

void AElement::_bCopy(const AElement &aeSource)
{
  //a_Do the parent copy
  AElementPairList::_bCopy(aeSource);

  delete[]m_pcTag;
  if (aeSource.m_pcTag)
    m_pcTag = aStrDup(aeSource.m_pcTag);
}

void AElement::eSetTag(const char *pccX)
{
  delete []m_pcTag;
  
  if (pccX)
    m_pcTag = aStrDup(pccX);
}

void AElement::doOut(AStreamOutput *pasOut) const
{
  //a_Main output function for the class
  assert(pasOut);
  if (pasOut)
  {
    if (m_pcTag)
    {
      //a_Start the tag
      pasOut->outStringN("<");
      pasOut->outString(m_pcTag);
      pasOut->outStringN(" ");
    }
  
    doPairs(pasOut, ' ', INT_MAX, 0x1);       //a_Output the list of key pairs
  
    //a_Close the tag
    if (m_pcTag)
      pasOut->outStringCRN(">");             
  }
}

//a_Ends this element </{m_pcTag>
void AElement::doEnd(AStreamOutput *pasOut) const   
{
  assert(pasOut);
  if (pasOut && m_pcTag)
  {
    pasOut->outStringN("</");
    pasOut->outString(m_pcTag);
    pasOut->outStringCRN(">");
  }
}
