#ifdef WIN32
#pragma message( "Compiling " __FILE__ )
#endif

//
// Some Random Number Generators were para-coded from a book:
//   Numerical Recipes in C: Cambrine University Press
//   An excellent reference book and a must if you do any scientific programming
//

#include "freeCGI.h"

#ifdef _DEBUG_DUMP_
void ARandom::dump(void)
{
  cout << endl << "---START: ARandom::dump()---";

  ABase::dump();
  cout << endl << "---END: ARandom::dump()---" << endl;
}
#endif

//a_Random Number Generators (RNG)
//a_Generates a random integer U[iMin, iMax] using PNG U[0.0, 1.0] specified
int ARandom::rngRandom(long &lSeed, int iMax, int iMin, PFN_DRNG pfnRNG)
{
  assert(this);

  if (!pfnRNG) pfnRNG = &ARandom::rngMinimal;

  //a_So far only works for positive values
  assert(iMin >= 0x0 && iMax >= 0x0);

  int iRet = iMin;
  if (iMax > iMin)
  {
    double dRandom = (this->*pfnRNG)(lSeed);     //a_Get our U(0,1)

    //a_Adjust so that every # is equaly likely
    dRandom *= (iMax - iMin + 0x1);     //a_Set the double into the range we need
    iRet = int(dRandom) + iMin;         //a_Round and offset
  }
  else
    assert(0x0);

  return iRet;
}

//a_"Minimal" RNG of Park and Miller
//a_Returns a uniform random deviate between 0.0 and 1.0
//a_Set/reset lSeed to any value except RNG_PMM_MASK
//a_For successive deviates in a sequence: do not alter the lSeed (it changes inside the function)
double ARandom::rngMinimal(long &lSeed)
{
  assert(this);

  long lK;
  static long lV[RNG_PMM_NTAB],
              lY = 0x0L;

  int iJ;

  //a_Initialize if the seed is negative
  if (lSeed <= 0x0L || !lY)
  {
    //a_Reset requested, generate a new seed!
    if (-lSeed < 0x1L) lSeed = 0x1L;
    else               lSeed = -lSeed;

    //a_Let's get ready to shuffle!  (8 round warm-up)
    for (iJ = RNG_PMM_NTAB + 0x7; iJ >= 0x0; iJ--)
    {
      lK = lSeed / RNG_PMM_IQ;
      lSeed = RNG_PMM_IA * (lSeed - lK * RNG_PMM_IQ) - lK * RNG_PMM_IR;
      if (lSeed < 0x0L) lSeed += RNG_PMM_IM;
      if (iJ < RNG_PMM_NTAB) lV[iJ] = lSeed;
    }

    lY = lV[0x0];
  }

  //a_Start to generate random number
  lK = lSeed / RNG_PMM_IQ;

  //a_Compute the seed without overflow using the Schrage's method
  lSeed = RNG_PMM_IA * (lSeed - lK * RNG_PMM_IQ) - RNG_PMM_IR * lK;
  if (lSeed < 0x0L) lSeed += RNG_PMM_IM;

  //a_Adjust with range for 0..NTAB-1
  iJ = lY / RNG_PMM_NDIV;
  lY = lV[iJ];
  lV[iJ] = lSeed;

  //a_Convert the seed to a double and return
  double dRet = lSeed;
  dRet *= RNG_PMM_AM0;
  dRet *= RNG_PMM_AM1;

  //a_Do not return an endpoint, it isn't expected
  if (dRet > RNG_PMM_RNMX)
  {
    assert(0x0);            //a_Just to see if we ever get here....
    return RNG_PMM_RNMX;
  }

  //a_Return our random #
  return dRet;
}

//a_Long period (> 2*(10^18)) L'Ecuyer with Bays-Durham shuffle
//a_Returns a U(0.0, 1.0) (which means it is end-point exclusive)
//a_Call with a negative # to reset, then use the same seed variable unaltered throughout
double ARandom::rngLEcuyer(long &lSeed)
{
  assert(this);

  long lK;
  static long lV[RNG_LEBD_NTAB],
              lY = 0x0L,
              lSeed2 = 123456789L;
  int iJ;

  //a_Initialize if the seed is negative
  if (lSeed <= 0x0L)
  {
    //a_Reset requested, generate a new seed!
    if (-lSeed < 0x1L) lSeed = 0x1L;
    else               lSeed = -lSeed;

    lSeed2 = lSeed;

    //a_Let's get ready to shuffle!  (8 round warm-up)
    for (iJ = RNG_LEBD_NTAB + 0x7; iJ >= 0x0; iJ--)
    {
      lK = lSeed / RNG_LEBD_IQ1;
      lSeed = RNG_LEBD_IA1 * (lSeed - lK * RNG_LEBD_IQ1) - lK * RNG_LEBD_IR1;
      if (lSeed < 0x0L) lSeed += RNG_LEBD_IM1;
      if (iJ < RNG_LEBD_NTAB) lV[iJ] = lSeed;
    }

    lY = lV[0x0];
  }

  //a_Start the generation of the random number
  lK = lSeed / RNG_LEBD_IQ1;
  lSeed = RNG_LEBD_IA1 * (lSeed - lK * RNG_LEBD_IQ1) - lK * RNG_LEBD_IR1;
  if (lSeed < 0x0L) lSeed += RNG_LEBD_IM1;

  lSeed2 = RNG_LEBD_IA2 * (lSeed2 - lK * RNG_LEBD_IQ2) - lK * RNG_LEBD_IR2;
  if (lSeed < 0x0L) lSeed += RNG_LEBD_IM2;

  //a_Now we shuffle
  iJ = lY / RNG_LEBD_NDIV;
  lY = lV[iJ] - lSeed2;
  lV[iJ] = lSeed;
  if (lY < 0x1L) lY += RNG_LEBD_IMM1;

  double dRet  = RNG_LEBD_AM0;
         dRet *= lY;
         dRet *= RNG_LEBD_AM1;

  //a_Do not return an endpoint, it isn't expected
  if (dRet > RNG_LEBD_RNMX)
  {
    assert(0x0);            //a_Just to see if we ever get here....
    return RNG_LEBD_RNMX;
  }

  return dRet;
}

BYTE *ARandom::rngGenerateRandomArray(long &lSeed, int iSize, PFN_DRNG pfnRNG)
{
  //a_If there is a size then generate key, else just clear things
  BYTE *pbRet = NULL;

  if (!pfnRNG) pfnRNG = &ARandom::rngMinimal;

  if (iSize > 0x0)
  {
    if (!lSeed)
    {
      //a_Use time()
      time((time_t*)&lSeed);        //a_Get the seconds since Jan 1, 1970
      lSeed = -lSeed;      //a_Invert to init the random routine
    }

    //a_Allocate an array and populate
    pbRet = aMemDup(NULL, iSize);
    if (pbRet)
    {
      for (int iX = 0x0; iX < iSize; iX++)
      {
        pbRet[iX] = rngRandom(lSeed, 0xFF, 0x0, pfnRNG);
      }
    }
  }

  return pbRet;
}

void ARandom::rngShuffleArray(long &lSeed, BYTE *pbArray, int iSize, PFN_DRNG pfnRNG)
{
  if (!pbArray || iSize < 0x0 || !pfnRNG)
  {
    //a_So much trouble in the world today...
    assert(0x0);
    return;
  }

  if (!pfnRNG) pfnRNG = &ARandom::rngMinimal;

  //a_Range is [0, iSize - 0x1]...
  iSize--;

  BYTE bSwap;
  int iSwap;
  for (register int iX = 0x0; iX < iSize; iX++)
  {
    //a_Get a random place for a swap
    iSwap = rngRandom(lSeed, iSize, 0x0, pfnRNG);

    //a_Swap values
    bSwap = pbArray[iSwap];
    pbArray[iSwap] = pbArray[iX];
    pbArray[iX] = bSwap;
  }
}

float ARandom::rngChiSquareTest(long &lSeed, int iN, int iRange, PFN_DRNG pfnRNG)
{
  if (!pfnRNG) pfnRNG = &ARandom::rngMinimal;

  //a_Our sample space is [0x0, N-1], hence N points
  int iX, iT, *iF;
  double dChi = 0.0;

  iF = new int[iRange];
  if (iF)
  {
    //a_Clear the sample space
    for (iX = 0x0; iX < iRange; iX++) iF[iX] = 0x0;

    //a_Take samples
    for (iX = 0x0; iX <= iN; iX++)
    {
      //a_Get a random place for a swap
      iT = rngRandom(lSeed, iRange - 0x1, 0x0, pfnRNG);
      assert(iT >= 0x0 && iT <= iRange - 0x1);   //a_Verify the functionality of the RNG
      iF[iT]++;                                  //a_Add add for that sample value

      #ifdef _DEBUG_FULL_
        cout << iT << ", ";
        if (0x0 == ((iX + 0x1) % 0x10)) cout << endl;
      #endif
    }

    double dProduct = 0.0;
    for (iX = 0x0; iX < iRange; iX++) dProduct += iF[iX] * iF[iX];

    dChi = dProduct * iRange / iN - iN;
    double dTest = dChi - iRange;
    dTest *= dTest;

    #ifdef _DEBUG_FULL_
      cout << endl << "dChi=" << dChi << "  (dTest=" << dTest << " < (4*iRange)=" << 4 * iRange << ")";

      //a_ChiSquare test: (dChi - iRange) within |2*sqtr(iRange)|
      if (dTest < 4 * iRange) cout << " ... test valid!" << endl;
      else                    cout << " ... test failed!" << endl;
    #endif

    delete []iF;
  }
  else
    assert(0x0);

  return dChi;
}

