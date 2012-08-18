#include "heap_min.h"

#define HEAP_INIT(l) do { \
	l->h = malloc(HEAP_MAX_SIZE * sizeof(heaparr)); \
	l->heap_size = 0; \
	srand((int)time(NULL)); \
} while(0)

#define HEAP_FREE(l) do { \
	if (l->h != NULL) \
		free(l->h); \
} while(0)

inline void __swap(heap* l, int index1, int index2) {
	if (unlikely(index1 == index2))
		return;
	HEAPIDX_ID(KEY(index1)) = index2;
	HEAPIDX_ID(KEY(index2)) = index1;
	int tmp = KEY(index1);
	KEY(index1) = KEY(index2);
	KEY(index2) = tmp;
}

inline void __move(heap* l, int to, int from) {
	if (unlikely(from == to))
		return;
	HEAPIDX_ID(KEY(from)) = to;
	KEY(to) = KEY(from);
	KEY(from) = 0; // invalid
}

inline void __adjust_heap(heap* l, int idx) {
	int left = (idx<<1) + 1;
	int right = (idx<<1) + 2;
	int smaller = (left < HEAP_SIZE(l) && VALUE(idx) > VALUE(left)) ?
		left : idx;
	smaller = (right < HEAP_SIZE(l) && VALUE(smaller) > VALUE(right)) ?
		right : smaller;
	
	if (idx != smaller) {
		__swap(l, idx, smaller);
		__adjust_heap(l, smaller);
	}
}

inline void __changed_node(heap* l, int id) {
	__adjust_heap(l, HEAPIDX_ID(id));
}

inline int __remove_node(heap* l, int id) {
	int retval = KEY(id);
	if (unlikely(HEAP_SIZE(l) == 0))
		return retval;
	HEAP_SIZE(l)--;
	__move(l, id, HEAP_SIZE(l));
	__adjust_heap(l, id);
	return retval;
}

inline int __remove_top(heap* l) {
	return __remove_node(l, 0);
}

#define RAND_RANGE(max) (rand() % (max))

inline int __remove_rand(heap* l) {
	return __remove_node(l, RAND_RANGE(HEAP_SIZE(l)));
}

inline void __insert_node(heap *l, int id) {
	int curr = HEAP_SIZE(l);
	while (curr > 0) {
		if (GET_HITS(id) >= VALUE(curr>>1))
			break;
		__move(l, curr, curr>>1);
		curr = curr>>1;
	}
	// ASSERT(GET_HITS(id) >= VALUE(curr>>1))
	HEAPIDX_ID(id) = curr;
	KEY(curr) = id;
	HEAP_SIZE(l)++;
}
