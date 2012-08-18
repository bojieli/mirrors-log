// Random
#include "cache_base.h"
#include "heap_min.h"

typedef heap_node rand_node;
typedef heap randl;

randl* cache_init_rand(int size) {
	CACHE_INIT(size, randl, rand_node)
	HEAP_INIT(cache);
	return cache;
}

void cache_free_rand(randl *l) {
	HEAP_FREE(l);
	CACHE_FREE(l);
}

bool cache_find_rand(randl *l, int id, UINT length) {
#define CACHE_HIT_HOOK(l,id) __changed_node(l, id)
	RETURN_CACHE_FIND(l, id, length);
#undef CACHE_HIT_HOOK
}

int cache_pop_rand(randl* l) {
	rand_node* poped = &(l->l[ __remove_rand(l) ]);
	RETURN_POP_CACHE(l, poped, sizeof(rand_node));
}

void cache_push_rand(randl* cache, int id, UINT filesize) {
	rand_node* node = &(cache->l[id]);
	PUSH_CACHE(cache, node, filesize);
	__insert_node(cache, id);
}

UINT cache_hits_rand(randl* l, int id) {
	RETURN_CACHE_HITS(l, id);
}

ULONG cache_size_rand(randl* l) {
	RETURN_CACHE_SIZE(l);
}

ULONG cache_req_rand(randl* l, int num) {
	RETURN_SUM_REQ(l, num);
}

ULONG cache_traffic_rand(randl* l, int num) {
	RETURN_SUM_TRAFFIC(l, num);
}
