//
//a_conv.h
//
// Conversion, Econding, Encryption, and such
//
#ifndef __A_CONV_H__
#define __A_CONV_H__

#ifdef WIN32
#pragma message( "Including " __FILE__ )
#endif

#include "a_predef.h"
#include "a_struct.h"
#include "a_base.h"

//a_Algorithm specific defines and such
#include "a_random.h"             

/////////////////////////////////////////////////////////////////////
// AConverto - conversion, encoding, etc.
/////////////////////////////////////////////////////////////////////
class AConverto : public ABase
{
  public:
	 AConverto();
	 virtual ~AConverto();

	 //a_Variables
	 enum enumConvertoType
	 {
		eatNone = 0x00000000, eat4Bit = 0x00000001, eat6Bit = 0x00000002,
		eatMask = 0x0000000F,
	 
		//a_Used for CRCs and checksums, many methods for added security
		//a_There are 3 versions: BYTE, WORD and DWORD return types
		ecrcXOR  = 0x00000100, ecrcModuloSum = 0x00000200, ecrcRNWCRC = 0x00000300,
		ecrcMask = 0x00000F00
	 };

    //a_Conversion routine wrappers
    int  convertoFromAlpha(UINT uType, char **pcInBlock, int iLength, int iReAllocate = 0x0);  //a_From alphanumeric to data
    int  convertoToAlpha(UINT uType, char **pcInBlock, int iLength, int iReAllocate = 0x1);    //a_From data to alphanumeric
    void convertoSetMap(const char *pccNewMap = NULL);                                         //a_NULL resets to the built in map

    //a_URL econding/decoding
    char *convertoEncodeURL(const char *pccSource, int iSourceLength = -1);
    char *convertoDecodeURL(const char *pccSource, int iZeroTest = 1);                         //a_iZeroTest will check and ignore %00 which translate into a NULL-terminator

    //aCounts # of bits in a BYTE
    int convertoBitCount(BYTE bTest)
    {
      static BYTE bCount[0x10] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};
      return (bCount[bTest & '\x0F'] + bCount[(bTest >> 0x4) & '\x0F']);
    }

    //a_HEX <-> BYTE conversions
    char *convertoBYTEtoHEX(BYTE byteV, char *pcRet);    //a_Creates a 2 BYTE HEX string from a BYTE value
    BYTE  convertoHEXtoBYTE(char cHi, char cLo);        //a_Creates a BYTE number from a 2 BYTE HEX string
    char *convertoHEXplusplus(char *pcRet);              //a_Increment a 2 BYTE hex # [00-FF]

    //a_COLORREF conversion
    char *convertoCOLORREFtoChar(COLORREF cr, char *pcRet);

    //a_Data processing, checksums, etc for data authentication and verification purposes
    //a_Accepts user CRCMODEL used for ecrcRNWCRC type
    BYTE  convertoCRCasBYTE(int iType, const char *pccData, int iDataLength, PCRCMODEL pcmUserModel = NULL);
    WORD  convertoCRCasWORD(int iType, const char *pccData, int iDataLength, PCRCMODEL pcmUserModel = NULL);
    DWORD convertoCRCasDWORD(int iType, const char *pccData, int iDataLength, PCRCMODEL pcmUserModel = NULL);

    //a_Mirrors the BYTE from LSB-MSB to MSB-LSB
    BYTE convertoMirrorBYTE(BYTE bX) const
    {
      //a_Mirror mapping:             0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF
      static BYTE s_bMirror[0x10] = { 0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE, 0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF };

      BYTE bRet  = s_bMirror[(bX & 0xF)] << 0x4;
           bRet |= (s_bMirror[((bX >> 0x4) & 0xF)] & 0xF);

      return bRet;
    }

    //a_Returns the value dwSource with the bottom iBits [0,32] bits reflected. Slower than convertoMirrorBYTE
    //a_Example: convertMirrorLowerDWORDBits(0x3e23L, 0x3) == 0x3e26
    //a_NOTE: convertoMirrorBYTE is much faster for mirroring BYTEs
    DWORD convertoMirrorLowerDWORDBits(DWORD dwSource, int iBits);

  protected:
    //-------------------------------------------------------------------------------------------------
    //a_Conversion routines to and from AlphaNumeric
    //-------------------------------------------------------------------------------------------------
    //a_4Bit routines convert hex value in stream to a 2 byte hex equivalent, (eg. AB -> \x41\x42), doubles buffer size in process
    int   _convertoTo4BitAlpha(char **, int, int iReAllocate = 0x1);   //a_Returns the new length of the buffer
    int   _convertoFrom4BitAlpha(char **, int, int iReAllocate = 0x0);      //a_Takes pointer to NULL terminated alphanumeric string, converts to data and returns length

    //-------------------------------------------------------------------------------------------------

    //a_6Bit routines
    int  _convertoTo6BitAlpha(char **, int, int iReAllocate = 0x1);                   //a_Returns the new length of the buffer
    int  _convertoFrom6BitAlpha(char **, int, int iReAllocate = 0x0);                 //a_Takes pointer to NULL terminated alphanumeric string, converts to data and returns length
    WORD _convertoMap6BitAlphaToWORD(BYTE bX, const char *pccMap, int iShift = 0x0)   //a_Mapping to WORD with shift from alphanumeric
    {
      assert(pccMap);
      const char *pccX = strchr(pccMap, bX);
      if (pccX)
        return (WORD(pccX - pccMap) << iShift);
      else
        return 0;

    }
    //a_6Bit mapping set (64 values = 2^6)
    //a_If you override and it contains non-alphanums then the purpose of these functions is changed
    //a_ it will act as a cypher-like-map and not an ASCII-map it is as a default  
    static const char sm_ccMap[65];        //a_Default is defined and static, can be overridden for security purpose
    char *m_pcUserMap;              //a_User can override the 64 character map (+1 for NULL)

    //-------------------------------------------------------------------------------------------------
    //a_CRC Model and it's initializers
    CRCMODEL m_crcModel;
    void  _convertoInit8BitCRC(void);
    void  _convertoInit16BitCRC(void);
    void  _convertoInit32BitCRC(void);
    void  _convertoDoBlockCRC(const char *pccBuffer, int iLength = -0x1);
    DWORD _convertoGetCurrentCRC(void);
};

//////////////////////////////////////////////////////////////////////
//ARandom - Random Number Generation object
//////////////////////////////////////////////////////////////////////
class ARandom : public ABase
{
  public:
    ARandom() {};
    virtual ~ARandom() {};

      //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

    //a_Park and Miller "Minimal" generator U(0,1) - uniform exclusive 0.0 to 1.0
    double rngMinimal(long &lSeed);

    //a_Long period L'Ecuyer with Bays-Durham shuffle (no it's not a dance!)
    //a_Takes almost twice as long as rngMinimal...
    double rngLEcuyer(long &lSeed);

    //a_Random # generators
    //a_Inclusive # in range U[iMin, iMax], true uniform distribution adjustment
    //a_Must supply a function to use as an RNG
    int rngRandom(long &lSeed, int iMax, int iMin = 0x0, double (ARandom::*pfRNG)(long &) = NULL);

    //a_Routine for generating an array of random BYTEs from a given seed, user must delete[] when done
    BYTE *rngGenerateRandomArray(long &lSeed, int iSize, double (ARandom::*pfRNG)(long &) = NULL);

    //a_Shuffle contents of a user provided array, (accepts a pointer to a function of tn RNG)
    void rngShuffleArray(long &lSeed, BYTE *pbArray, int iSize, double (ARandom::*pfRNG)(long &) = NULL);

    //a_Chi-Square test (ported from "Algorithms in C" by R. Sedgewick; p.517)
    //a_N is the number of random samples to generate and test
    //a_iRange is the [0, iRange) to use in this test
    float rngChiSquareTest(long &lSeed, int iN, int iRange, double (ARandom::*pfRNG)(long &) = NULL);
};

//a_Generic function definition for a RNG
typedef double (ARandom::*PFN_DRNG)(long &lSeed);

////////////////////////////////////////////////////////////////
// ACrypto - encryption class
////////////////////////////////////////////////////////////////
class ACrypto : virtual public AConverto, virtual public ARandom
{
  public:
    //a_Construction and destruction
    ACrypto();
    virtual ~ACrypto();
    
    //a_Declares debug/dump related functions
    #ifdef _DEBUG_DUMP_
      void dump(void);     //a_Debugging dump, when _DEBUG_DUMP_ is set
    #endif

    //a_Key control
    int cryptoSetKey(const BYTE *pcbNewKey, int iLength);       //a_Assign a new key
    const BYTE *cryptoGetKey(int &iLength)
    { iLength = m_iKeyLength; return m_pbKey; }          //a_Pointer to the key (NULL if none)
    
    //a_Key and string generation using random variables (lSeed == 0x0 means use time())
    //a_lSeed should be negative (see ARandom for more details)
    //a_Returns the seed value used so you may continue this random series
    long cryptoSetRandomKey(int iSize, long lSeed = 0x0L);

    //a_Variables
    enum enumCypherType
    {
      ectNone = 0x00000000, ectXOR_Blitter = 0x00000010, ectXOR_Convolver = 0x00000020,
      ectXOR_ShiftingConvolver = 0x00000040,
		ectMask = 0x000000F0
	 };
	 
    //a_Stream encryptor wrappers, accepts a buffer, works on it and returns it's pointer for convenience or new length
    int cryptoEncryptOnly(UINT uType, char *pcInBlock, int iLength);
    char *cryptoDecryptOnly(UINT uType, char *pcInBlock, int iLength);

    //a_Main wapper functions
    //a_D/Encryptor routines that convert to/from AlphaNumeric stream not BYTE data
    int cryptoEncrypt(UINT uType, char **pcInBlock, int iLength = -1, int iReAllocate = 0x1);
    int cryptoDecrypt(UINT uType, char **pcInBlock, int iLength, int iReAllocate = 0x0);
    
  protected:
    //a_Key
    BYTE *m_pbKey;
    int   m_iKeyLength;
    
    //-------------------------------------------------------------------------------------------------
    //a_Encryption/Decryption Algorithms
    //-------------------------------------------------------------------------------------------------
    //a_XOR Blitter
    //a_Uses keys sequentially to just do XORs, very fast but not that secure, no shifting just a blit-like process
    //a_Does both encryption and decryption due to non-shifting property
    char *_StandardXORBlit(char *pcInBlock, int iLength);
    //-------------------------------------------------------------------------------------------------
    //a_XOR Convolver
    //a_Uses all the keys and shifts(convolves) them with the input
    char *_ConvolvingXOREncrypt(char *pcInBlock, int iLength);
    char *_ConvolvingXORDecrypt(char *pcInBlock, int iLength);
    //-------------------------------------------------------------------------------------------------
    //a_XOR Shifting Convolver
    //a_Uses all the keys and shifts(convolves) them with the input with a key shifted by the input
    char *_ShiftingConvolvingXOREncrypt(char *pcInBlock, int iLength);
    char *_ShiftingConvolvingXORDecrypt(char *pcInBlock, int iLength);
    //-------------------------------------------------------------------------------------------------
};

static ACrypto g_aCrypto;   //a_1 global object always exists for use by hierarchy

#endif

