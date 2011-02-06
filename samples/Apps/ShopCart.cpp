//
//AShopping Cart example
//

//a_How I set up my shopping cart
//a_On the web page output for changing:
//    NAME="{CartName}{Action}{Quantity to Add}" VALUE="{My Internal Part Number}"
//a_On the web page in a hidden field (current contents)
//    NAME="{CartName}{Current Quantity} VALUE="{My Internal Part Number}"
//a_In the AShoppingCart class item
//    NAME="{My Internal Part Number}" VALUE="(null)" Quantity={Quantity}
//
//a_Why this set up? Well it is easy to parse from FORM items when getting a submission and
//  easy to maintain the AShoppingCart item list and quantity
//
//a_Feel free to derive your own configuration if this doesn't work for you.

//#define _DEBUG_DUMP_
//#define _DEBUG_FULL_
//#define _TEST_

#include "freeCGI.h"

#define SC_NAME             "ITEM"

#define SC_SUBMIT_SEPARATOR '_'
#define SC_ADD              "ADD"
#define SC_REMOVE           "DEL"
#define SC_LENGTH           0x3      //a_Length of the strings for add/remove (must be the same!)

void showItemTable(ACGI &cgiOut);

int main()
{
  ACGI cgiOut;

  cgiOut.mimeHTML();
  cgiOut.htmlStartHTML();
  cgiOut.htmlDoTITLE("The Apple Cart!");

  cgiOut.htmlStartBODY();

  cgiOut.htmlDoTagEx("H1", "ALIGN=CENTER", "The Store Of No More");

  //a_Set up the shopping cart from form items
  AShoppingCart asCart;
  asCart.ascSetCartName(SC_NAME);          //a_Parses all things that start with ITEMxx (note you can have many different shopping carts)

  #ifdef _TEST_
    cgiOut.cgiGetFormItems("ITEM_ADD01=RRA&ITEM.1=RRA&Send=Sand%21");    //a_Arbitrary input for testing
  #else
    cgiOut.cgiGetFormItems();               //a_Get all form items
  #endif

  //a_Using the APairList parent of ACGI form items, find items of this cart
  asCart.ascFindCartItems(cgiOut);        

  //a_Start the submission form
  cgiOut << "<FORM ACTION=\"" << cgiOut.cgiGetScriptName() << "\"><BR>" << endl;

  //a_Show possible items
  showItemTable(cgiOut);

  //a_Add item string  {CartName}{AddName}
  char sTemp[32];
  strcpy(sTemp, SC_NAME);
//  strcat(sTemp, SC_ADD);

  //a_Now add new items
  const char *pccName;
  APairItem *ppiX = NULL;
  ASCartItem *psciAdded;
  while (ppiX = CAST(APairItem *, cgiOut.plGetItemByName(sTemp, ppiX, 0x1)))
  {
    pccName = ppiX->piGetName() + strlen(sTemp);

    //a_Now check if we are adding or removing
    int iQ = 0x0;
    if ((*pccName == SC_SUBMIT_SEPARATOR) && (strlen(pccName) > SC_LENGTH))      //a_Must be add/remove type
    {
      *pccName++;
      if (0x0 == strncmp(pccName, SC_ADD, SC_LENGTH))
      {
        //a_Adding items
        pccName += SC_LENGTH;
        iQ = atoi(pccName);
      
        #ifdef _DEBUG_FULL_
          cout << "<!--Adding " << iQ << " -->" << endl;
        #endif
      }
      else
      {
        if (0x0 == strncmp(pccName, SC_REMOVE, SC_LENGTH))
        {
          //a_Removing items
          pccName += SC_LENGTH;
          iQ = - atoi(pccName);
        }
      }

      #ifdef _DEBUG_FULL_
        cout << "<!--Trying to add " << iQ << " of " << ppiX->piGetValue() << " -->" << endl;
      #endif

   
      if ((iQ) && (psciAdded = asCart.ascAddCartItem(ppiX->piGetValue(), iQ)))
      {
        psciAdded->piSetValue("Some Description");
      }
    }

    ppiX = CAST(APairItem *, ppiX->diGetNext());
  } 

  //a_Embed the shoppind cart
  cgiOut << asCart;
  cgiOut << "<INPUT TYPE=SUBMIT NAME=\"Send\" VALUE=\"Sand!\">" << endl;
  cgiOut << "</FORM><BR>" << endl;

  //a_Contents of the cart so far
  ASCartItem *psciX = CAST(ASCartItem *, asCart.lGetHead());
  if (psciX)
  {
    cgiOut.htmlStartTag("BIG");
    cgiOut.htmlDoTag("B", "Shopping Cart Contents");
    cgiOut.htmlEndTag("BIG");
    
    cgiOut.htmlStartTag("UL");
    while(psciX)
    {
      cgiOut << "<LI>" << *psciX << "iQ=" << psciX->sciGetQuantity();
      psciX = CAST(ASCartItem *, psciX->diGetNext()); 
    }
    cgiOut.htmlEndTag("UL");
  }

#ifdef _DEBUG_DUMP_
  cgiOut << "<PRE>" << endl;
  asCart.dump();    //a_Do a dump...
  cgiOut << "</PRE>" << endl;
#endif

  cgiOut.htmlEndBODY();
  cgiOut.htmlEndHTML();

  return 0x0;
}

void showItemTable(ACGI &cgiOut)
{
  //a_I'll use {CartName}{AddName}{Quantity} for adding to a specific cart, but anything can be used
  //a_ NAME ={CartName}{Action}
  //a_ VALUE={Your part # (ASCII please :)}
  //a_NOTE: This is not how I stored the data in the AShoppingCart class (although you are free to do as you like...)

  cgiOut << "<TABLE BORDER=1>" << endl;
  cgiOut << "<TR><TH COLSPAN=3>Items Of Questionable Use</TH></TR>" << endl;
  cgiOut << "<TR><TH>Add 1</TH><TH>Remove 1</TH><TH>Item Description</TH></TR>" << endl;
  
  AElementPairList eplInput;
  
  cgiOut << "<TR><TD>";
    eplInput.plAddItem("VALUE", "RRA");
      //a_Checkbox to add
      eplInput.plAddItem("NAME", "ITEM_ADD01");
      cgiOut.cgiDoFORMInput(FORMINPUT_CHECKBOX, eplInput);
    cgiOut << "</TD><TD>";
      //a_Checkbox to remove
      eplInput.plAddItem("NAME", "ITEM_DEL01");
      cgiOut.cgiDoFORMInput(FORMINPUT_CHECKBOX, eplInput);
  cgiOut << "</TD><TD>Red Rubberrized Apple</TD></TR>" << endl;

  cgiOut << "<TR><TD>";
    eplInput.plAddItem("VALUE", "SAT");
      //a_Checkbox to add
      eplInput.plAddItem("NAME", "ITEM_ADD01");
      cgiOut.cgiDoFORMInput(FORMINPUT_CHECKBOX, eplInput);
    cgiOut << "</TD><TD>";
      //a_Checkbox to remove
      eplInput.plAddItem("NAME", "ITEM_DEL01");
      cgiOut.cgiDoFORMInput(FORMINPUT_CHECKBOX, eplInput);
  cgiOut << "</TD><TD>Shiny Aluminum Toothpick +1</TD></TR>" << endl;

  cgiOut << "<TR><TD>";
    eplInput.plAddItem("VALUE", "RBRS");
      //a_Checkbox to add
      eplInput.plAddItem("NAME", "ITEM_ADD01");
      cgiOut.cgiDoFORMInput(FORMINPUT_CHECKBOX, eplInput);
    cgiOut << "</TD><TD>";
      //a_Checkbox to remove
      eplInput.plAddItem("NAME", "ITEM_DEL01");
      cgiOut.cgiDoFORMInput(FORMINPUT_CHECKBOX, eplInput);
  cgiOut << "</TD><TD>Rustproof Bronze Running Shoes</TD></TR>" << endl;

  cgiOut << "<TR><TD>";
    eplInput.plAddItem("VALUE", "IEFD");
      //a_Checkbox to add
      eplInput.plAddItem("NAME", "ITEM_ADD01");
      cgiOut.cgiDoFORMInput(FORMINPUT_CHECKBOX, eplInput);
      //a_Checkbox to remove
    cgiOut << "</TD><TD>";
      eplInput.plAddItem("NAME", "ITEM_DEL01");
      cgiOut.cgiDoFORMInput(FORMINPUT_CHECKBOX, eplInput);
  cgiOut << "</TD><TD>Inorganic Edible Floppy Disks</TD></TR>" << endl;

  cgiOut << "<TR><TD>";
    eplInput.plAddItem("VALUE", "SRRC");
      //a_Checkbox to add
      eplInput.plAddItem("NAME", "ITEM_ADD01");
      cgiOut.cgiDoFORMInput(FORMINPUT_CHECKBOX, eplInput);
    cgiOut << "</TD><TD>";
      //a_Checkbox to remove
      eplInput.plAddItem("NAME", "ITEM_DEL01");
      cgiOut.cgiDoFORMInput(FORMINPUT_CHECKBOX, eplInput);
  cgiOut << "</TD><TD>Slammo Reusable Radial Condom</TD></TR>" << endl;

/*
  cgiOut << "<TR><TD><INPUT TYPE=CHECKBOX NAME=\"ITEM_ADD01\" VALUE=\"RRA\"></TD><TD>Red Rubberrized Apple</TD></TR>" << endl;
  cgiOut << "<TR><TD><INPUT TYPE=CHECKBOX NAME=\"ITEM_ADD01\" VALUE=\"SAT\"></TD><TD>Shiny Aluminum Toothpick +1</TD></TR>" << endl;
  cgiOut << "<TR><TD><INPUT TYPE=CHECKBOX NAME=\"ITEM_ADD01\" VALUE=\"RBRS\"></TD><TD>Rustproof Bronze Running Shoes</TD></TR>" << endl;
  cgiOut << "<TR><TD><INPUT TYPE=CHECKBOX NAME=\"ITEM_ADD01\" VALUE=\"IEFD\"></TD><TD>Inorganic Edible Floppy Disks</TD></TR>" << endl;
  cgiOut << "<TR><TD><INPUT TYPE=CHECKBOX NAME=\"ITEM_ADD01\" VALUE=\"SRRC\"></TD><TD>Slammo Reusable Radial Condom</TD></TR>" << endl;
*/
  
  cgiOut << "</TABLE>" << endl;
}
