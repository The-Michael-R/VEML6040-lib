/**
 * Just a small debug print routine which prevents debug code to be present
 * in the release code.
 */

#ifndef DEBUG_UTIL_H
#define DEBUG_UTIL_H

#include <stdio.h>

#ifdef DEBUG
 #define DEBUG_PRINT(x)     printf(x);
 #define DEBUG_PRINTDEC(x, y)  printf(x, y);
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTDEC(x)
#endif

#endif /* DEBUG_UTIL_H */
