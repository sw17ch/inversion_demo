#ifndef defines_h
#define defines_h

#include <string.h>

#define IGNORE(V) ((void)V)
#define OBJ_ZERO(O) (memset(&O, 0, sizeof(O)))
#define MIN(A,B) ((A) < (B) ? (A) : (B))

#endif /* defines_h */
