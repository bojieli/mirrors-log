#ifndef CACHE_COMMON
#define CACHE_COMMON

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))

typedef struct {
	void* (*init)(int);
	void (*free)(void*);
	int (*find)(void*, int, UINT);
	int (*pop)(void*);
	void (*push)(void*, int, UINT);
	UINT (*hits)(void*, int);
	ULONG (*size)(void*);
	ULONG (*req)(void*, int);
	ULONG (*traffic)(void*, int);
	const char* algo_name;
} cache_func_entry;

#endif
