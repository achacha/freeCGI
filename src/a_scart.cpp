
#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

#include "freeCGI.h"

//a_How I set up my shopping cart
//a_On the web page output for changing:
//    NAME="{CartName}{Action}{Quantity to Add}" VALUE="{My Internal Part Number}"
//a_On the web page in a hidden field (current contents)
//    NAME="{CartName}{Current Quantity} VALUE="{My Internal Part Number}"
//a_In the AShoppingCart class item
//    NAME="{My Internal Part Number}" VALUE="{Total Quantity}"
//
//a_Why this set up? Well it is easy to parse from FORM items when getting a submission and
//  easy to maintain the AShoppingCart item list and quantity
//

#ifdef _DEBUG_DUMP_
void AShoppingCart::dump(void)
{
  //a_Only works if the _DEBUG_DUMP_ flag is set and MIME directive was sent (ie. Content-Type: text/html\n\n)
  cout << endl << "---START: APairList::dump()---";
  AList::dump();

  ASCartItem *psciX = CAST(ASCartItem *, lGetHead());
  while (psciX)
  {
    psciX->dump();
    psciX = CAST(ASCartItem *, psciX->diGetNext());
  }
  cout << endl << "------END: APairList::dump()------" << endl;
}
#endif

AShoppingCart::AShoppingCart(AShoppingCart *pscSource)
{
  if (pscSource)
    _bCopy(*pscSource);
  else
  {
    m_pcCartName = NULL;
  }
}

AShoppingCart::~AShoppingCart()
{
  delete []m_pcCartName;
}

void AShoppingCart::doOut(AStreamOutput *pasOut) const
{
  assert(pasOut);
  if (pasOut)
  {
    //a_These MUST be embedded in a <FORM>-type of a submission, since it creates hidden types...
    ASCartItem *psciX = CAST(ASCartItem *, lGetHead());
    char sNum[32];
    strcpy(sNum, "00");

    while (psciX)
    {
      *pasOut << "<INPUT TYPE=HIDDEN NAME=\"";
      
      itoa(psciX->sciGetQuantity(), sNum, 0xA);
      *pasOut << ascGetCartName() << ASC_SEPARATOR << sNum;               
      
      *pasOut << "\" VALUE=\"";
      
      *pasOut << psciX->piGetName();      //a_NOTE: the VALUE of this item is not used, just for internals if needed
      *pasOut << "\"><BR>" << endl;

      psciX = CAST(ASCartItem *, psciX->diGetNext());     //a_Next cart item
    }
  }
}

//a_This function locates all fields that doOut creates (not the new ones)
void AShoppingCart::ascSetCartName(const char *pccName)
{
  delete []m_pcCartName;
  m_pcCartName = aStrDup(pccName);
}

int AShoppingCart::ascFindCartItems(const APairList &plSource)
{
  if (!m_pcCartName)
  {
    assert(0x0);         //a_No cart name assigned!
    return 0x0;         
  }

  int iFound = 0x0, iCNL = strlen(m_pcCartName);
  APairItem *ppiX = NULL;

  while (ppiX = plSource.plGetItemByName(m_pcCartName, ppiX, 0x1))
  {
    //a_Found a cart item
    const char *pccX = ppiX->piGetName();
    if (pccX[iCNL] == ASC_SEPARATOR)
    {
      //a_Seems valid so far
      int iQ = atoi(&pccX[iCNL + 0x1]);

      ASCartItem *psciNew = CAST(ASCartItem *, plGetItemByName(ppiX->piGetValue()));    //a_See if the current part # exists
      if (psciNew)
      {
        #ifdef _DEBUG_FULL_
          cout << "<!--AShoppingCart::ascFindCartItems: Found " << iQ << " of " << ppiX->piGetValue() << "-->" << endl;
        #endif
        *psciNew += iQ;
      }
      else
      {
        if (psciNew = new ASCartItem)
        {
          #ifdef _DEBUG_FULL_
            cout << "<!--AShoppingCart::ascFindCartItems: Creating " << iQ << " of " << ppiX->piGetValue() << "-->" << endl;
          #endif

          psciNew->piSet(ppiX->piGetValue());
          psciNew->sciSetQuantity(iQ);
          lAdd(psciNew);
        }
      }
    }
    
    //a_Advance to next and continue search, if nothing left, break out...
    if (!(ppiX = CAST(APairItem *, ppiX->diGetNext())))
      break;
  }

  return iFound;
}

void AShoppingCart::_bCopy(const AShoppingCart &scSource)
{
  ABaseElement::_bCopy(scSource);   //a_Parent copy (do not call other list's _bCopy functions)

  ASCartItem *psciNew, *psciX = CAST(ASCartItem *, lGetHead());
  while (psciX)
  {
    psciNew = new ASCartItem(psciX);       //a_Copy constructor
    lAdd(psciNew);
    psciX = CAST(ASCartItem *, psciX->diGetNext());
  }

  //a_Copy the cart's name
  ascSetCartName(aStrDup(scSource.ascGetCartName()));
}

ASCartItem *AShoppingCart::ascAddCartItem(const char *pccItemName, int iQuantity)
{
  ASCartItem *psciX = NULL;
  if (pccItemName)
  {
    psciX = CAST(ASCartItem *, plGetItemByName(pccItemName));
    if (psciX)
    {
      //a_Already exists, just increase quantity
      *psciX += iQuantity;
    }
    else
    {
      //a_Need to add a new item
      psciX = new ASCartItem;

      if (psciX)
      {
        //a_Set and add
        psciX->piSet(pccItemName);
        psciX->sciSetQuantity(iQuantity);
        lAdd(psciX);
      }
    }
  }
  else
    assert(0x0);    //a_No item name?

  return psciX;
}
