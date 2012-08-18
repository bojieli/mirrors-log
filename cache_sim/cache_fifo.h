// FIFO
// just a bit different from LRU: do not move to tail when accessing
#include "cache_base.h"
#include "linked_list.h"

// double linked list
typedef struct fifo_node {
	bool cached;
	UINT filesize;
	UINT hits; // hits since NOT cached
	ULONG req; // total cached requests
	ULONG traffic; // total cached traffic
	struct fifo_node* prev;
	struct fifo_node* next;
} fifo_node;

typedef struct {
	ULONG cache_size;
	fifo_node* l; // array, node 0 as sentinel
} fifo;

fifo* cache_init_fifo(int size) {
	CACHE_INIT(size, fifo, fifo_node)
	LIST_INIT(cache->l);
	return cache;
}

void cache_free_fifo(fifo *l) {
	CACHE_FREE(l);
}

bool cache_find_fifo(fifo *cache, int id, UINT length) {
#define CACHE_HIT_HOOK(cache,id)
	RETURN_CACHE_FIND(cache, id, length);
#undef CACHE_HIT_HOOK
}

// node 0's next is head, prev is tail
// pop from head, push to tail
int cache_pop_fifo(fifo* cache) {
	fifo_node* poped = cache->l->next;
	LIST_REMOVE_NODE(poped);
	RETURN_POP_CACHE(cache, poped, sizeof(fifo_node));
}

void cache_push_fifo(fifo* cache, int id, UINT filesize) {
	fifo_node* node = &(cache->l[id]);
	LIST_INSERT_TO_TAIL(cache->l, node);
	PUSH_CACHE(cache, node, filesize);
}

UINT cache_hits_fifo(fifo *l, int id) {
	RETURN_CACHE_HITS(l, id);
}

ULONG cache_size_fifo(fifo* l) {
	RETURN_CACHE_SIZE(l);
}

ULONG cache_req_fifo(fifo *l, int num) {
	RETURN_SUM_REQ(l, num);
}

ULONG cache_traffic_fifo(fifo *l, int num) {
	RETURN_SUM_TRAFFIC(l, num);
}
