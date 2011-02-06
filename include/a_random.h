////////////////////////////////////////////////////
//
// a_random.h - Header file for a_random.hpp
//
//   Defines and constants used by RNG algorithms
//

#ifndef __A_RANDOM_H__
#define __A_RANDOM_H__

#ifdef WIN32
#pragma message( "Including " __FILE__ )
#endif

//a_"Minimal" RNG of Park and Miller with Bays-Durham shuffle
#define RNG_PMM_IA    16807L
#define RNG_PMM_IM    2147483647L
#define RNG_PMM_AM0   0.4656612875246     //a_AM0 * AM1 = (1.0 / RNG_PMM_IM1)
#define RNG_PMM_AM1   1e-9
#define RNG_PMM_IQ    127773L
#define RNG_PMM_IR    2836L
#define RNG_PMM_NTAB  32
#define RNG_PMM_NDIV  67108861L           //a_(RNG_LEBD_IM1/RNG_LEBD_NTAB)
#define RNG_PMM_EPS   2e-016              
#define RNG_PMM_RNMX  0.999999999999999   

//a_L'Ecuyer long period RNG with Bays-Durham shuffle
#define RNG_LEBD_IQ1  53668L
#define RNG_LEBD_IQ2  52774L
#define RNG_LEBD_IR1  12211L
#define RNG_LEBD_IR2  3791L
#define RNG_LEBD_IA1  40014L
#define RNG_LEBD_IA2  40692L
#define RNG_LEBD_IM1  2147483563L
#define RNG_LEBD_IM2  2147483399L
#define RNG_LEBD_IMM1 2147483562L         //a_RNG_LEBD_IM1 - 1
#define RNG_LEBD_AM0  0.4656613057392     //a_AM0 * AM1 = (1.0 / RNG_LEBD_IM1)
#define RNG_LEBD_AM1  1e-9 
#define RNG_LEBD_NTAB 32
#define RNG_LEBD_NDIV 67108861L           //a_(RNG_LEBD_IM1/RNG_LEBD_NTAB)
#define RNG_LEBD_EPS  2e-016              
#define RNG_LEBD_RNMX 0.9999999999999999  

#endif
