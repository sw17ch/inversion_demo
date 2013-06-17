#ifndef defines_h
#define defines_h

#include <string.h>

/* Mark an identifier as ignored. */
#define IGNORE(V) ((void)V)

/* Set an object's memory to 0 bytes. */
#define OBJ_ZERO(O) (memset(&O, 0, sizeof(O)))

/* Return the minimum of either A or B. */
#define MIN(A,B) ((A) < (B) ? (A) : (B))

#endif /* defines_h */
