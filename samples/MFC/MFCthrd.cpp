#include <afx.h>
#include <iostream.h>
#include "freeCGI.h"


DWORD __stdcall uSleeperThread(LPVOID lpvParam);
typedef DWORD (_stdcall *PF_THREAD)(LPVOID pftFunction);

#define THREAD_COUNT 9
class AThreader
{
  public:
    AThreader(CPtrArray *pcpaContainer = NULL)
    {
      ASSERT(this);
      
      if (pcpaContainer) 
        m_pcpaContainer = pcpaContainer;
      
      if (sm_lSeed == 0x0L)
        Randomize();
    
      m_iThreadNumber = -1;
      m_hThread = 0x0;
      m_dwThreadID = 0x0L;
    }
  
    ~AThreader()
    {
    }

    static void SetACGI(ACGI *pcgiOut) { sm_pcgiOut = pcgiOut; }
    int GetThreadCount(void) { return sm_iThreadCount; }
    
    void Randomize(long lMask = 0x0)
    {
      sm_lSeed = (long)::GetCurrentTime() ^ lMask;
      sm_lSeed *= ((sm_lSeed > 0x0) * -1L);
    }

    int tCreateThread(PF_THREAD pftFunction)
    {
      if (!pftFunction) return 0x0;
      
      if (m_hThread = ::CreateThread(NULL, 0x400, pftFunction, (LPVOID)this, 0x0, &m_dwThreadID))
      {
        //a_Success
        m_iThreadNumber = sm_iThreadCount++;
        m_pcpaContainer->SetAt(m_iThreadNumber, (LPVOID)this);
        return 0x1;
      }
      else
        return 0x0;
    }

    int  GetCount(void) { return sm_iThreadCount; }
    int  GetIndex(void) { return m_iThreadNumber; }
    void TimeToDie(void)
    {
      //a_Removes self from queue
      m_pcpaContainer->SetAt(m_iThreadNumber, NULL);
      
      //a_Delete item
      delete this;

      //a_Decrement count
      sm_iThreadCount--;
    }

    static long      sm_lSeed;
    static ACGI     *sm_pcgiOut;
    
    HANDLE           m_hThread;

  protected:
    static int       sm_iThreadCount;
    
    CPtrArray *m_pcpaContainer;
    DWORD      m_dwThreadID;
    int        m_iThreadNumber;
};

int   AThreader::sm_iThreadCount = 0x0;
long  AThreader::sm_lSeed        = 0x0L;
ACGI *AThreader::sm_pcgiOut      = NULL;


//a_Gloabal thread storage
static CPtrArray cpaContainer;

int main()
{
  ACGI cgiOut;
  AThreader::SetACGI(&cgiOut);
 
  AThreader *ptPeon;
  cpaContainer.SetSize(THREAD_COUNT);

  CString csOut = "Content-Type: text/html\x00A\x00A";
  cgiOut << csOut;

  cgiOut << "<HTML><HEAD><TITLE>Hello World!</TITLE></HEAD>" << endl;
  cgiOut << "<BODY><H1 ALIGN=CENTER>Hello World...</H1>" << endl;


  static long s_lCount =0x0;
  long lLast;

  for (int iX = 0x0; iX < THREAD_COUNT; iX++)
  {
    ptPeon = new AThreader(&cpaContainer);

    cgiOut << "<B>Creating delay thread #" << iX << "</B><BR>" << endl;
    if (!ptPeon->tCreateThread(&uSleeperThread))
    {
      cgiOut << "<I>Thread #" << iX << ": creation failed with error = " <<  GetLastError() << "<I><BR>" << endl;
      
      goto CLEANUP;
    }
    else
    {
    }
  //  ::SetThreadPriority(hThread, THREAD_PRIORITY_BELOW_NORMAL);    //a_Background thread
  }

  lLast = ptPeon->GetCount();
  while (ptPeon->GetCount() > 0x0)
  {
  }

  cgiOut << "<BR><B>All threads finished</B><BR>";

  cgiOut << "</BODY></HTML>" << endl;

CLEANUP:
  for (iX = 0x0; iX < cpaContainer.GetSize(); iX++)
  {
    if (ptPeon = (AThreader *)cpaContainer.GetAt(iX))
    {
      delete ptPeon;
      cpaContainer.SetAt(iX, NULL);
      
      #ifdef _DEBUG_DUMP_
        cgiOut << "<SMALL>Thread #" << iX << " did not exit!</SMALL><BR>" << flush;
      #endif
    }
  }
  
  return 0x1;
}

DWORD __stdcall uSleeperThread(LPVOID lpvParam)
{
  AThreader *ptPeon = ((AThreader *)lpvParam);

  //a_Random sleep
  long lX = 600 + g_aCrypto.rngRandom(ptPeon->sm_lSeed, 2000);
  
  if (ptPeon->sm_pcgiOut)
    *(ptPeon->sm_pcgiOut) << "<BR><B>Thread#" << ptPeon->GetIndex() << " about to sleep for " << lX << " ms</B><BR>" << endl;
  
  while (lX > 0x0L)
  {
    if (ptPeon->sm_pcgiOut)
      *(ptPeon->sm_pcgiOut) << ptPeon->GetIndex() << flush;
    Sleep(200);  //a_Delay for N milliseconds
    lX -= 200;
  }

  if (ptPeon->sm_pcgiOut)
    *(ptPeon->sm_pcgiOut) << "<BR><B>Thread#" << ptPeon->GetIndex() << " about to die...</B><BR>" << endl;

  ptPeon->TimeToDie();      //a_Decrease thread count

  return 0x0;
} 
