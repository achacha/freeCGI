#ifndef __A_LIST_H__
#define __A_LIST_H__

#ifdef WIN32
#pragma message( "Including " __FILE__ )
#endif

#include "a_predef.h"
#include "a_base.h"
#include "a_stream.h"

//////////////////////////////////////////
// ADataItem - basic item of AList (a linked list)
//////////////////////////////////////////
class ADataItem : public ABase
{
  public:
    ADataItem() { m_padiNext = NULL; }
    virtual ~ADataItem() {}

    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

    //a_Setting
    void       diSetNext(ADataItem *padiNext) { m_padiNext = padiNext; }
    ADataItem *diGetNext(void) { return m_padiNext; }

  protected:
    void _bCopy(const ADataItem &diSource) {}

    ADataItem *m_padiNext;
};

/////////////////////////////////////////////////////////
// AList - A linked list
/////////////////////////////////////////////////////////
class AList : public ABase
{
  public:
    AList()          { m_padiHead = NULL; }
    virtual ~AList() { lKill(); }

    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

    //a_Access members
    int         lIsEmpty(void) { return (m_padiHead ? 0x0 : 0x1); } //a_Empty is it has no head (the headless list!) 
    int         lGetCount(void) const;                              //a_Iterates and counts
    ADataItem  *lGetHead(void) const { return m_padiHead; }         //a_Great name! :)
    ADataItem  *lGetAt(int iIndex) const;                           //a_Pointer at offset, NULL if invalid, HEAD is negative
    ADataItem  *lGetLast(void) const;                               //a_Itreate to end and return last item's pointer

    //a_List manipulators
    void lAdd(ADataItem *padiNew);
    void lInsertAfter(ADataItem *padiNew, ADataItem *padiAfter = NULL);

    //a_List cleaning
    void lRemove(ADataItem *padiRemove);
    void lRemove(int iIndex);
    void lKill(int iStartIndex = 0x0);

  protected:
    ADataItem *m_padiHead;
};

//////////////////////////////////////////////////////////////
// AStateHeader - contains state information from/to HTML
//////////////////////////////////////////////////////////////
#define SH_MARK 0xDEAD         //a_16bit constant to check is the header is not dead :) (Ah, the irony of life)
class AStateHeader
{
  public:
    DWORD  dwCRC32;   //a_CRC32 for the data
    DWORD  dwUserID;  //a_DWORD that can ID user (0x0 means it is not important), IP is great for this
    time_t ttTime;    //a_When the header was created, used for timeout  (note: time_t is usually a long, 4 BYTES)
    WORD   wLength;   //a_Length of data to follow
    WORD   wMark;     //a_Must be 0xFADE to verify decrypted header is valid

    AStateHeader(const BYTE *pcbUserData = NULL, int iUserDataLength = -0x1, DWORD dwUserID = 0x0);
    void Init(const BYTE *pcbUserData, int iUserDataLength, DWORD dwNewID = 0x0L);
    int GetElapsedSeconds(void) const;      //a_Uses current time() and ttTime to get the differnce
    int IsValid(void) const { return (wMark == SH_MARK); }
    int IsEqual(AStateHeader &shSource, int iTimeout = -0x1);
};

////////////////////////////////////////////////////////////////////////////////
// APairItem class of NAME=VALUE type usable in forms, params, etc...
////////////////////////////////////////////////////////////////////////////////
class APairItem : virtual public ABaseElement, virtual public ADataItem
{
  public:
    APairItem(APairItem *ppiSource = NULL);
    virtual ~APairItem();

    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

    //a_ABaseElement pure virtual override
    virtual void doOut(AStreamOutput *pasOut) const;
    
    //a_Access functions (getXXX inlined for speed)
    const char *piGetName(void) const  { return m_pcName; }
    const char *piGetValue(void) const { return m_pcValue; }
    
    //a_Set functions  (piSet(NULL) clear the pair)
    int  piSet(const char *pccName, const char *pccValue = NULL);
    void piSetValue(const char *pccValue);   //a_Replace a VALUE in this item

    //a_Encoded
    void piSetValueEncoded(const BYTE *pcbUserData, int iLength, UINT uMethod = AConverto::eat4Bit, const BYTE *pcbKey = NULL, int iKeyLength = 0x0);
    const BYTE *piDecodeValueAndGetUserData(int &iLength, UINT uMethod = AConverto::eat4Bit, const BYTE *pcbKey = NULL, int iKeyLength = 0x0);

    //a_Checked variable (embeds CRC32, length, ID etc...)
    //a_User data and length contain state info
    //a_ dwIP if provided is used to doublecheck remote (dwIP can be anything that should be consistent at time of making the state variable and pasring it)
    void piSetValueChecked(const BYTE *pcbUserData, int iUserDataLength, DWORD dwUserID = 0x0, UINT uMethod = AConverto::eat6Bit, const BYTE *pcbKey = NULL, int iKeyLength = 0x0);
    const BYTE *piDecodeCheckedValueAndGetUserData(int &iUserDataLength, DWORD dwUserID = 0x0, int iTimeout = -0x1, UINT uMethod = AConverto::eat6Bit, const BYTE *pcbKey = NULL, int iKeyLength = 0x0);

    //a_Parameter control
    DWORD piIsFlag(DWORD dwCheck) const    { return (dwCheck & m_dwParam); }
    void  piSetFlag(DWORD dwSet)           { m_dwParam |= dwSet; }
    void  piClearFlag(DWORD dwClear = 0x0) { m_dwParam &= ~dwClear; }

  protected:
    //a_Copy function
    void _bCopy(const APairItem &piSource);

    //a_Pair dependent flag
    DWORD m_dwParam;

    //a_Key pair
    char *m_pcName;
    char *m_pcValue;

    //a_Used for encoding/decoding element members (no need to copy, used as persistent return values)
    BYTE *m_pbUserData;           //a_User data
    WORD  m_wUserDataLength;      //a_User data length
};

///////////////////////////////////////////////////////////////////////////////////
// APairList class
///////////////////////////////////////////////////////////////////////////////////
class APairList : virtual public AList, virtual public ABaseElement
{
  public:
    APairList() {}
    virtual ~APairList() {}

    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

    //a_ABaseElement pure virtual override
    virtual void doOut(AStreamOutput *pasOut) const
      { doPairs(pasOut); }
    void doPairs(AStreamOutput *pasOut, char cSeparator = ' ', int iItemsPerLine = INT_MAX, int iQuoted = 0x0, int iURLEncode = 0x0) const;
    void doPairs(AStreamOutput *pasOut, const char *pccSeparator, int iItemsPerLine = INT_MAX, int iQuoted = 0x0, int iURLEncode = 0x0) const;

    //a_List access/operators  (Partial is part of pccName at start of item name, and Anywhere finds pccName anywhere in item name)
    const char *plGetValueByName(const char *pccName, APairItem *ppiStart = NULL, BYTE bPartial = 0x0, BYTE bAnywhere = 0x0) const;
    APairItem  *plGetItemByName(const char *pccName, APairItem *ppiStart = NULL, BYTE bPartial = 0x0, BYTE bAnywhere = 0x0) const;    
    APairItem  *plCreateNewItem(const char *pccName, const char *pccValue = NULL);         //a_Only creates, does not add!
    APairItem  *plAddItem(const char *pccName, const char *pccValue = NULL, int iReplace = 0x1);  //a_Adds to end of list a pair of NAME and VALUE
    void        plRemoveItemByName(const char *pccName, int iRemoveAllSameName = 0x1);

  protected:
    void _bCopy(const APairList &plSource);
};

#endif

