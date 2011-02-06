#ifndef __A_BASE_H__
#define __A_BASE_H__

#ifdef WIN32
#pragma message( "Including " __FILE__ )
#endif

class AStreamOutput;

///////////////////////////////////////////////////////////////////////////////////
// ABase class
///////////////////////////////////////////////////////////////////////////////////
class ABase
{
  public:
    ABase() {}
    virtual ~ABase() {}

    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

  protected:
    void _bCopy(const ABase &) {};   //a_No members yet...
};


//////////////////////////////////////////////////////////////////////
// ABaseElement class provides output to AStreamOutput
//////////////////////////////////////////////////////////////////////
class ABaseElement : public ABase
{
  public:
    ABaseElement() {};
    virtual ~ABaseElement() {};
    
    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

    //a_Used by all children of the ABaseElement (the family heirloom of sorts :)
    virtual void doOut(AStreamOutput *pasOut) const = 0;

  protected:
    void _bCopy(const ABaseElement &beSource) { ABase::_bCopy(beSource); }
};

#endif

