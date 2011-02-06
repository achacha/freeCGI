
#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

#include "freeCGI.h"

#ifdef _DEBUG_DUMP_
void AStreamOutput::dump(void)
{
  cout << endl << "---START: AStreamOutput::dump()---";
  ABase::dump();                                 //a_Do parent dump as well
  cout << "m_posOut = " << m_posOut << endl;
  cout << "---END: AStreamOutput::dump()---" << endl;
}
#endif

#if defined(_WINDOWS_) && defined(_MSC_VER)  //a_Used only if Visual C++ is used with CWinApp
//a_stdout can be text or binary, so if iSetOutStreamBinary != 0 for binary output
//a_Binary output is useful for outputting non-text based MIME-types (no \n -> \n\r translation)
int AStreamOutput::streamCreateConsoleStdio(int iSetOutStreamBinary)
{
  //a_Handles for stdin and stdout and FILE handle when they are opened
  int iCRT;
  FILE *fileHandle;

  //a_Works when executed from console or file manager type, not program manager
  if (!AllocConsole())
  {
    #ifdef __AFX_H__
    TRACE0("Unable to connact to a console session.\r\n");
    #endif
    return 0x0;
  }
  
  //a_Create stdout
  iCRT = _open_osfhandle((long) GetStdHandle(STD_OUTPUT_HANDLE), (iSetOutStreamBinary ? _O_BINARY : _O_TEXT));
  if ((fileHandle = _fdopen(iCRT, "w")) == NULL)
  {  
    #ifdef __AFX_H__
    TRACE0("Unable to open stdout for write.\r\n");
    #endif
    return 0x0;
  }
  else
    *stdout = *fileHandle;
  
  //a_Create stdin
  iCRT = _open_osfhandle((long) GetStdHandle(STD_INPUT_HANDLE), _O_TEXT);
  if ((fileHandle = _fdopen(iCRT, "r")) == NULL)
  {
    #ifdef __AFX_H__
    TRACE0("Unable to open stdin for read.\r\n");
    #endif
    return 0x0;
  }
  else
    *stdin = *fileHandle;

  //a_Create stderr
  iCRT = _open_osfhandle((long) GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
  if ((fileHandle = _fdopen(iCRT, "w")) == NULL)
  {
    #ifdef __AFX_H__
    TRACE0("Unable to open stdin for read.\r\n");
    #endif
    return 0x0;
  }
  else
    *stderr = *fileHandle;

  //a_Attach stdio to buffers
  if (setvbuf(stdout, NULL, _IONBF, 0) || 
      setvbuf(stdin,  NULL, _IONBF, 0) ||
      setvbuf(stderr, NULL, _IONBF, 0))
  {
    #ifdef __AFX_H__
    TRACE0("Invalid or illegal buffer specified to stdin/stdout/stderr.\r\n");
    #endif
    return 0x0;
  }

  //a_Re-sync cout/cin/cerr with stdin/stdout/stderr
  ios::sync_with_stdio();

  return 0x1;
}
#endif


