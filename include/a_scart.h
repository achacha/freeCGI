//a_Shopping Cart related objects
#ifndef __A_SCART_H__
#define __A_SCART_H__

#ifdef WIN32
#pragma message( "Including " __FILE__ )
#endif

#include "a_list.h"

////////////////////////////////////////////////////////////////////////////////
// ASCartItem - AShoppingCart item
////////////////////////////////////////////////////////////////////////////////
class ASCartItem : public APairItem
{
  public:
    ASCartItem(ASCartItem *psciSource = NULL);
    virtual ~ASCartItem();

    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

    //a_ABaseElement pure virtual override
    virtual void doOut(AStreamOutput *pasOut) const;

    //a_Quantity control
    int  sciGetQuantity(void) const     { return m_iQuantity; }
    void sciSetQuantity(int iNewQ)      { m_iQuantity = iNewQ; }
    ASCartItem &operator ++(void)       { assert(m_iQuantity < INT_MAX); m_iQuantity++; return *this; }
    ASCartItem &operator --(void)       { if (m_iQuantity > 0x00) m_iQuantity--; return *this; }
    ASCartItem &operator +=(int iAddQ);

  protected:
    //a_Copy function
    void _bCopy(const ASCartItem &sciSource);

    int m_iQuantity;                         //a_How many of these
};

///////////////////////////////////////////////////////////////////////////////////
// AShoppingCart class
///////////////////////////////////////////////////////////////////////////////////
class AShoppingCart : public APairList
{
  public:
    AShoppingCart(AShoppingCart *pscSource = NULL);
    virtual ~AShoppingCart();

    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

    //a_Used by all children of the ABaseElement (the family heirloom of sorts :)
    virtual void doOut(AStreamOutput *pasOut) const;

    //a_Cart info
    void        ascSetCartName(const char *pccName);
    const char *ascGetCartName(void) const { return m_pcCartName; }

    //a_Use a PairList and get items specific to this cart
    int ascFindCartItems(const APairList &plSource);

    //a_Maintenance
    ASCartItem *ascAddCartItem(const char *pccItemName, int iQuantity = 0x1);

  protected:
    void _bCopy(const AShoppingCart &scSource);

    char *m_pcCartName;
};

#endif

