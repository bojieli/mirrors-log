// least recently used
#include "cache_base.h"
#include "linked_list.h"

// double linked list
typedef struct lru_node {
	bool cached;
	UINT filesize;
	UINT hits; // hits since NOT cached
	ULONG req; // total cached requests
	ULONG traffic; // total cached traffic
	struct lru_node* prev;
	struct lru_node* next;
} lru_node;

typedef struct {
	ULONG cache_size;
	lru_node* l; // array, node 0 as sentinel
} lru;

lru* cache_init_lru(int size) {
	CACHE_INIT(size, lru, lru_node)
	LIST_INIT(cache->l);
	return cache;
}

void cache_free_lru(lru *l) {
	CACHE_FREE(l);
}

#ifdef DEBUG_VERBOSE
void print_lru_list(lru *l) {
	fprintf(stderr, "LRUlist: ");
	lru_node* head = l->l;
	lru_node* curr = head;
	while (curr->next != head) {
		curr = curr->next;
		fprintf(stderr, "%d ", curr - head);
	}
	fprintf(stderr, "\n");
}
#endif

bool cache_find_lru(lru *cache, int id, UINT length) {
#ifdef DEBUG_VERBOSE
	fprintf(stderr, "id:%d length:%d hits:%d cached:%d req:%d traffic:%d\n", id, length, cache->l[id].hits, cache->l[id].cached, cache->l[id].req, cache->l[id].traffic);
#endif
#define CACHE_HIT_HOOK(cache,id) LIST_MOVE_TO_TAIL(cache->l, &(cache->l[id]))
	RETURN_CACHE_FIND(cache, id, length);
#undef CACHE_HIT_HOOK
}

// node 0's next is head, prev is tail
// pop from head, push to tail
int cache_pop_lru(lru* cache) {
	lru_node* poped = cache->l->next;
#ifdef DEBUG_VERBOSE
	fprintf(stderr, "Pop id:%d filesize:%d cachesize:%lu\n", poped - cache->l, poped->filesize, cache->cache_size);
#endif
	LIST_REMOVE_NODE(poped);
#ifdef DEBUG_VERBOSE
	print_lru_list(cache);
#endif
	RETURN_POP_CACHE(cache, poped, sizeof(lru_node));
}

void cache_push_lru(lru* cache, int id, UINT filesize) {
	lru_node* node = &(cache->l[id]);
#ifdef DEBUG_VERBOSE
	fprintf(stderr, "Push id:%d filesize:%d cachesize:%lu\n", id, filesize, cache->cache_size);
#endif
#ifdef DEBUG_VERBOSE
	print_lru_list(cache);
#endif
	LIST_INSERT_TO_TAIL(cache->l, node);
#ifdef DEBUG_VERBOSE
	print_lru_list(cache);
#endif
	PUSH_CACHE(cache, node, filesize);
}

UINT cache_hits_lru(lru *l, int id) {
	RETURN_CACHE_HITS(l, id);
}

ULONG cache_size_lru(lru* l) {
	RETURN_CACHE_SIZE(l);
}

ULONG cache_req_lru(lru *l, int num) {
	RETURN_SUM_REQ(l, num);
}

ULONG cache_traffic_lru(lru *l, int num) {
	RETURN_SUM_TRAFFIC(l, num);
}
