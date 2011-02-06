/////////////////////////////////////////////////////////////
//
//  Random Number Tester
//

#define _DEBUG_DUMP_
#define _DEBUG_FULL_

#include "freeCGI.h"

#define TEST_STRING "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define TEST_LENGTH 36

//a_Test that prints out N random numbers from a given RNG in U(0,1)
void testRNGDouble(ARandom &aRandom, PFN_DRNG pfRNG, long lSeed, int iTrials);

//a_Test that will print an integer in U(0,31)
void testRNG(ARandom &aRandom, PFN_DRNG pfRNG, long lSeed, int iTrials);

//a_Generates an XBM of random points set (almost like a stereogram :)
void testXBM(ARandom &aRandom, PFN_DRNG pfRNG, long lSeed);

//a_Shuffle test
void testShuffle(ARandom &aRandom, PFN_DRNG pfRNG, long lSeed, BYTE *pbBuffer, int iSize);

//a_Test the RNGs with a Chi-Square test
void testUsingChiSquare(ARandom &aRandom, PFN_DRNG pfRNG, long lSeed, int iTrials, int iRange);

int main()
{
  ARandom aRNG;
  
  time_t ttSeed;
  time(&ttSeed);

  //a_Uncomment one of the tests below...

//-----------------------------------------------------------------
//  testRNGDouble(aRNG, aRNG.rngMinimal, -ttSeed, 6400);
//-----------------------------------------------------------------
//  testRNGDouble(aRNG, aRNG.rngLEcuyer, -ttSeed, 6400);
//-----------------------------------------------------------------

//-----------------------------------------------------------------
//  testRNG(aRNG, aRNG.rngMinimal, -ttSeed, 6400);
//-----------------------------------------------------------------
//  testRNG(aRNG, aRNG.rngLEcuyer, -ttSeed, 6400);
//-----------------------------------------------------------------

//-----------------------------------------------------------------
//  testXBM(aRNG, aRNG.rngMinimal, -ttSeed);
//-----------------------------------------------------------------
//  testXBM(aRNG, aRNG.rngLEcuyer, -ttSeed);
//-----------------------------------------------------------------


//a_Chi Sqaured test for RNGs
//-----------------------------------------------------------------
  testUsingChiSquare(aRNG, aRNG.rngMinimal, -ttSeed, 2048, 100);
//-----------------------------------------------------------------
  testUsingChiSquare(aRNG, aRNG.rngLEcuyer, -ttSeed, 2048, 100);
//-----------------------------------------------------------------


//-----------------------------------------------------------------
/*
  //a_Shuffle test
  BYTE *pbData = new BYTE[256];

    cout << "---Minimal Shuffler---" << endl;
    strcpy((char *)pbData, TEST_STRING);
    testShuffle(aRNG, aRNG.rngMinimal, -ttSeed, pbData, TEST_LENGTH);
    
    cout << "--L'Ecuyer Shuffler---" << endl;
    strcpy((char *)pbData, TEST_STRING);
    testShuffle(aRNG, aRNG.rngLEcuyer, -ttSeed, pbData, TEST_LENGTH);
    
    cout << "---Random Seed Shuffle---" << endl;
    for (int iA = 0x0; iA < 0xA; iA++)
    {
      strcpy((char *)pbData, TEST_STRING);
      ttSeed = - aRNG.rngRandom(ttSeed, LONG_MAX);
      testShuffle(aRNG, aRNG.rngLEcuyer, -ttSeed, pbData, TEST_LENGTH);
    }

    cout << "---Same seed test---" << endl;
    strcpy((char *)pbData, TEST_STRING);
    testShuffle(aRNG, aRNG.rngLEcuyer, -1969, pbData, TEST_LENGTH);
    strcpy((char *)pbData, TEST_STRING);
    testShuffle(aRNG, aRNG.rngLEcuyer, -1969, pbData, TEST_LENGTH);

  delete []pbData;
*/
//-----------------------------------------------------------------

  return 0x1;
}

void testRNGDouble(ARandom &aRandom, PFN_DRNG pfRNG, long lSeed, int iTrials)
{
  int iBreak = 0x1;
  double dX;
  for (int iT = 0x0; iT < iTrials; iT++, iBreak++)
  {
    dX = (aRandom.*pfRNG)(lSeed);
    cout << dX << ", ";
    if ((iBreak % 0x4) == 0x0)
    {
      cout << endl;
      iBreak = 0x0;
    }
  }
}

void testRNG(ARandom &aRandom, PFN_DRNG pfRNG, long lSeed, int iTrials)
{
 
  int iT;
  const int iiX = 32;
  DWORD dwArray[iiX];
  for (iT = 0x0; iT < iiX; iT++)
    dwArray[iT] = 0x0;

  //a_Set random coordinate
  int iX;
  for (iT = 0x0; iT < iTrials; iT++)
  {
    iX = aRandom.rngRandom(lSeed, iiX - 0x1, 0x0, pfRNG);
    cout << "{" << iT << "}=" << iX << endl;
    dwArray[iX]++;
  }

  cout << "{";
  for (iT = 0x0; iT < iiX; iT++)
  {
    cout << dwArray[iT] << ", ";
  }
  cout << "}" << endl;
}



void testXBM(ARandom &aRandom, PFN_DRNG pfRNG, long ttSeed)
{
  long ttSeedX = ttSeed,
       ttSeedY = (ttSeed / 3);   //a_Reset upon startup (or just a negative seed :)
  
  const int iiX = 128, iiY = 128;

  //a_Create a bit matrix to set bits in
  ABitMatrix bmRandom;
  bmRandom.mSetSize(iiX, iiY);
  bmRandom.mSetPlane();

  //a_Set random coordinate
  int iX, iY;
  for (int iT = 0x0; iT < iiX * iiY; iT++)
  {
    iX = aRandom.rngRandom(ttSeedX, iiX - 0x1, 0x0, pfRNG);
    iY = aRandom.rngRandom(ttSeedY, iiY - 0x1, 0x0, pfRNG);

    bmRandom[iX][iY] = 0x1;
  }
 
  //a_Invert bitmap (uncomment to invert the bitmap)
//  bmRandom.mSetPlane(0xFF, BLIT_XOR);
  
  //a_Create the output X11 bitmap that will accept our bit matrix
  AXBitmap xbmOut;
  xbmOut.mimeXBitmap();
  xbmOut.xbmDoBitmap(bmRandom);

}

void testShuffle(ARandom &aRandom, PFN_DRNG pfRNG, long lSeed, BYTE *pbBuffer, int iSize)
{
  cout << "Before=" << pbBuffer << endl;
  aRandom.rngShuffleArray(lSeed, pbBuffer, iSize, pfRNG);
  cout << "After =" << pbBuffer << endl;
}

void testUsingChiSquare(ARandom &aRandom, PFN_DRNG pfRNG, long lSeed, int iTrials, int iRange)
{
  cout << "dChi=" << aRandom.rngChiSquareTest(lSeed, iTrials, iRange, pfRNG) << endl;
}
