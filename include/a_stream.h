//a_stream.h - output classes
#ifndef __A_STREAM_H__
#define __A_STREAM_H__

#ifdef WIN32
#pragma message( "Including " __FILE__ )
#endif

#include "a_predef.h"
#include "a_base.h"

///////////////////////////////////////////////////////////////////////////////////
// AStreamOutput class, includes encryption
///////////////////////////////////////////////////////////////////////////////////
class AStreamOutput : public ABase
{
  public:
    AStreamOutput(ostream *posOut = NULL) { m_posOut = (posOut ? posOut : &cout); }
    ~AStreamOutput() {};

    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

    //a_Output to ostream variable m_posOutput, by default cout
    void setOStream(ostream *posNewOut)
    {
      if (posNewOut) m_posOut = posNewOut;
    }
    ostream *getOStream(void) { return m_posOut; }

    //a_Standard stream support with assert() checks
    AStreamOutput &operator<< (char cOut)   { assert(m_posOut); *m_posOut << cOut;  return *this; }
    AStreamOutput &operator<< (short nOut)  { assert(m_posOut); *m_posOut << nOut;  return *this; }
    AStreamOutput &operator<< (long lOut)   { assert(m_posOut); *m_posOut << lOut;  return *this; }
    AStreamOutput &operator<< (BYTE bOut)   { assert(m_posOut); *m_posOut << bOut;  return *this; }
    AStreamOutput &operator<< (WORD wOut)   { assert(m_posOut); *m_posOut << wOut;  return *this; }
    AStreamOutput &operator<< (DWORD dwOut) { assert(m_posOut); *m_posOut << dwOut; return *this; }
    AStreamOutput &operator<< (int iOut)    { assert(m_posOut); *m_posOut << iOut;  return *this; }
    AStreamOutput &operator<< (UINT uOut)   { assert(m_posOut); *m_posOut << uOut;  return *this; }
    AStreamOutput &operator<< (float fOut)  { assert(m_posOut); *m_posOut << fOut;  return *this; }
    AStreamOutput &operator<< (double dOut) { assert(m_posOut); *m_posOut << dOut;  return *this; }
    AStreamOutput &operator<< (const char *pccOut)
      { assert(m_posOut); outString(pccOut); return *this; }
    AStreamOutput &operator<< (signed char scOut)
      { assert(m_posOut); *m_posOut << scOut;  return *this; }
    AStreamOutput &operator<< (void *pv)
      { assert(m_posOut); *m_posOut << pv; return *this; }
    AStreamOutput &operator<< (streambuf *psb)
      { assert(m_posOut); *m_posOut << psb; return *this; }
    AStreamOutput &operator<< (ostream& (*fcn)(ostream&))
      { assert(m_posOut); *m_posOut << fcn; return *this; }
    AStreamOutput &operator<< (ios& (*fcn)(ios&))
      { assert(m_posOut); *m_posOut << fcn; return *this; }

    //a_Stream wrappers
    void outString(const char *pccOut)
    {
      assert(m_posOut);
      if (pccOut)
        *m_posOut << pccOut;
      else
        *m_posOut << "(null)";
    }
    void outStringN(const char *pccOut)  //a_For speed does no check
    {
      assert(m_posOut);
      *m_posOut << pccOut;
    }
    void outStringQ(const char *pccOut)  //a_Add quotes
    {
      assert(m_posOut);
      if (pccOut)
        *m_posOut << "\"" << pccOut << "\"";
      else
        *m_posOut << "(null)";
    }
    void outStringCR(const char *pccOut)
    {
      assert(m_posOut);
      if (pccOut)
        *m_posOut << pccOut;
      else
        *m_posOut << "(null)";
    
      *m_posOut << endl;            //a_With end-of-line added
    }
    void outStringCRN(const char *pccOut)
    {
      assert(m_posOut);
      *m_posOut << pccOut << endl;
    }

    #if defined(_WINDOWS) && defined(_MSC_VER)  //a_Used only if Visual C++ is used with CWinApp
      #pragma message("CWinApp definition found: streamCreateConsoleStdio() is available.")
      //a_Allocates a console session and attaches iostream to stdio of the console
      int streamCreateConsoleStdio(int iSetOutStreamBinary = 0x0);
    #endif

  protected:
    ostream *m_posOut;

    //a_Raw dump to the output stream (what ever it was set to)
};

#endif

