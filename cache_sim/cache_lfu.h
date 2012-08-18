// least frequently used
#include "cache_base.h"
#include "heap_min.h"

typedef heap_node lfu_node;
typedef heap lfu;

lfu* cache_init_lfu(int size) {
	CACHE_INIT(size, lfu, lfu_node)
	HEAP_INIT(cache);
	return cache;
}

void cache_free_lfu(lfu *l) {
	HEAP_FREE(l);
	CACHE_FREE(l);
}

bool cache_find_lfu(lfu *l, int id, UINT length) {
#define CACHE_HIT_HOOK(l,id) __changed_node(l, id);
	RETURN_CACHE_FIND(l, id, length);
#undef CACHE_HIT_HOOK
}

int cache_pop_lfu(lfu* l) {
	lfu_node* poped = &(l->l[ __remove_top(l) ]);
	RETURN_POP_CACHE(l, poped, sizeof(lfu_node));
}

void cache_push_lfu(lfu* cache, int id, UINT filesize) {
	lfu_node* node = &(cache->l[id]);
	PUSH_CACHE(cache, node, filesize);
	__insert_node(cache, id);
}

UINT cache_hits_lfu(lfu* l, int id) {
	RETURN_CACHE_HITS(l, id);
}

ULONG cache_size_lfu(lfu* l) {
	RETURN_CACHE_SIZE(l);
}

ULONG cache_req_lfu(lfu *l, int num) {
	RETURN_SUM_REQ(l, num);
}

ULONG cache_traffic_lfu(lfu *l, int num) {
	RETURN_SUM_TRAFFIC(l, num);
}
