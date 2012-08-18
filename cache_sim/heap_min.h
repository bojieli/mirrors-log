#ifndef HEAP_MIN
#define HEAP_MIN

#define HEAP_MAX_SIZE (1<<20)

typedef int heaparr;

typedef struct {
	bool cached;
	UINT filesize;
	UINT hits;
	ULONG req;
	ULONG traffic;
	int heapidx;
} heap_node;

typedef struct {
	ULONG cache_size;
	heap_node* l;
	heaparr* h;
	int heap_size;
} heap;

#define HEAP(cache) (cache->h)
#define HEAPIDX(node) (node->heapidx)
#define HEAPIDX_ID(id) (l->l[id].heapidx)
#define GET_HITS(id) (l->l[id].hits)
#define HEAP_SIZE(cache) (cache->heap_size)
#define KEY(index) (l->h[index])
#define VALUE(index) GET_HITS(KEY(index))

inline void __swap(heap*, int, int);
inline void __move(heap*, int, int);
inline void __adjust_heap(heap*, int);
inline void __changed_node(heap*, int);
inline int __remove_node(heap*, int);
inline int __remove_top(heap*);
inline int __remove_rand(heap*);
inline void __insert_node(heap*, int);

#include "heap_min.c"
#endif
