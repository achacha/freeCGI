#ifndef __A_FORM_H__
#define __A_FORM_H__

#ifdef WIN32
#pragma message( "Including " __FILE__ )
#endif

#include "a_predef.h"
#include "a_stream.h"
#include "a_list.h"

///////////////////////////////////////////////////////////////////////////////////
// AFormItem - node to a linked list to contain the form Value
///////////////////////////////////////////////////////////////////////////////////
class AFormItem : public APairItem
{
  public: 
    AFormItem(AFormItem *pfiSource = NULL)
    {
      if (pfiSource)
        _bCopy(*pfiSource);               //a_Copy the form item
    }
    
    virtual ~AFormItem() {}

    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

    //a_FORM specific parsing, NAME=VALUE format with URL encoding
    int fiSetAndParse(const char *pccItem);         //a_Parses the input NAME=VALUE type

  protected:
    void _bCopy(const AFormItem &fiSource);
};

///////////////////////////////////////////////////////////////////////////////////
// AFormList class
///////////////////////////////////////////////////////////////////////////////////
class AFormList : public APairList
{
  public:
    AFormList();
    AFormList(const AFormList&);
    virtual ~AFormList();

    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

    //a_ABaseElement pure virtual override
    virtual void doOut(AStreamOutput *pasOut) const
      { doPairs(pasOut, '&', INT_MAX, 0x0, 0x1); }

    //a_AFormItem list generation, returns the count of items
    int flGenerateList(istream *pisInput, int iContentLength);  //a_Stream processing
    int flGenerateList(const char *pccInput);                   //a_String processing

    //a_List manipulators, if replace is set, only one VALUE per NAME, else many can exist
    AFormItem *flAddItem(const char *pccItem, int iReplace = 0x0);                        //a_Adds to the end of the list with parsing of NAME=VALUE

    const char *getSafeFormData() { return (m_pccFormData ? m_pccFormData : NULL_STRING); }

  protected:
    void _bCopy(const AFormList &flSource);

    int _flProcessInput(istringstream *pssInput, int iContentLength);

  private:
      char *m_pccFormData;
};

#endif

