#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef ULONG
#define ULONG unsigned long
#endif
#ifndef UINT
#define UINT  unsigned int
#endif

#ifndef STATUS
#define STATUS int
#endif
#define TRUE 1
#define FALSE 0

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x)   __builtin_expect(!!(x), 0)
