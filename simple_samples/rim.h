#ifndef rim_H_
#define rim_H_
 
 
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <avr/pgmspace.h>
 
#define rim_NUM_CELLS 427
#define rim_SAMPLERATE 16384
 
const int8_t __attribute__((section(".progmem.data"))) rim_DATA [] = {1, -2, 2, -7, -11, 3, 6, 3, 5, -2,
-42, -33, -28, -10, 38, 50, 36, 44, -13, -38, -26, -14, -15, 14, 63, 58, 31,
-49, -28, -19, -16, -14, 17, 63, 32, 39, -9, -99, -41, -30, -2, 45, 24, 30, 8,
19, -39, -68, -45, -17, 27, 22, 19, 20, -15, -40, -25, -20, -8, -4, 12, 25, 13,
14, 16, 16, 14, 16, 11, 41, 57, 37, -4, -85, -50, -15, 12, 35, 20, 16, 5, -2,
-11, -21, -35, -20, -21, -18, -19, -12, -28, -44, -28, -16, -2, 7, 18, 24, 29,
31, 33, 30, 30, 25, 22, 17, 15, 11, 4, -7, -10, -6, 0, 20, 15, 5, -5, -14, -20,
-26, -29, -31, -29, -27, -24, -18, -21, -28, -23, -12, -1, 8, 17, 23, 27, 30,
31, 30, 27, 25, 20, 15, 11, 8, 3, 0, -3, -5, -4, -2, 2, 6, 1, -5, -11, -16, -19,
-22, -22, -22, -20, -18, -20, -18, -17, -11, -4, 2, 8, 13, 17, 20, 23, 23, 21,
20, 18, 15, 11, 9, 5, 2, -1, -2, -3, -4, -4, -5, -4, -4, -5, -6, -7, -8, -9,
-11, -12, -12, -13, -13, -12, -9, -6, -3, 0, 4, 7, 9, 11, 13, 13, 13, 12, 12,
10, 9, 6, 4, 3, 1, 0, -2, -2, -3, -4, -3, -3, -4, -5, -5, -6, -6, -7, -7, -7,
-7, -6, -6, -5, -4, -2, 0, 1, 3, 4, 5, 6, 6, 7, 7, 6, 6, 5, 4, 3, 3, 2, 0, 0,
-1, -1, -2, -2, -2, -3, -3, -3, -3, -4, -4, -5, -5, -3, -3, -3, -3, -3, -1, -1,
1, 1, 2, 3, 4, 3, 5, 5, 3, 5, 3, 3, 2, 2, 2, 2, 1, 0, 0, -1, -1, -1, -2, -2, -3,
-3, -3, -3, -2, -3, -3, -2, -3, -1, -1, -1, 0, 0, 1, 1, 1, 2, 3, 2, 3, 2, 2, 2,
3, 2, 2, 1, 2, 0, 0, 0, -1, 0, -2, -1, -1, -2, -2, -2, -2, -3, -2, -2, -2, -2,
-1, 0, 0, 0, 1, 0, 1, 1, 2, 2, 1, 1, 2, 1, 2, 1, 2, 1, 0, 0, 0, 0, 0, -1, -1, 0,
-1, -1, -1, -1, -1, -1, -1, 0, 0, -1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0,
0, 0, 0, -1, 0, 0, 0, -1, -1, -1, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, };

#endif /* rim_H_ */
