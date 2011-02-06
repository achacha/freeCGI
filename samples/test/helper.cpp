// Testing of the helper functions

#include "freeCGI.h"

int main(int argc, char **argv)
{
  char buffer[] = "This is a test. \x10 \x12 \x14 \x16";
  ACGI out;


  AHTMLHelper::dumpBinaryDataAsHex(out, buffer, strlen(buffer), 8);
  AHTMLHelper::dumpBinaryDataAsPrintable(out, buffer, strlen(buffer), 8);

  return 0;
}
