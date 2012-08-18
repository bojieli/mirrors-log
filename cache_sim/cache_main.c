#include "cache_main.h"

const cache_func_entry cache_funcs[] = {
	{cache_init_lru, cache_free_lru, cache_find_lru, cache_pop_lru, cache_push_lru, cache_hits_lru, cache_size_lru, cache_req_lru, cache_traffic_lru, "LRU"},
	{cache_init_lfu, cache_free_lfu, cache_find_lfu, cache_pop_lfu, cache_push_lfu, cache_hits_lfu, cache_size_lfu, cache_req_lfu, cache_traffic_lfu, "LFU"},
	{cache_init_fifo, cache_free_fifo, cache_find_fifo, cache_pop_fifo, cache_push_fifo, cache_hits_fifo, cache_size_fifo, cache_req_fifo, cache_traffic_fifo, "FIFO"},
	{cache_init_rand, cache_free_rand, cache_find_rand, cache_pop_rand, cache_push_rand, cache_hits_rand, cache_size_rand, cache_req_rand, cache_traffic_rand, "Random"}
};

void generic_cache(void* l, int id, UINT filesize, UINT req_length, cache_func_entry* funcs, CACHE_SAVE_FUNC_PROTO(save_func)) {
	if ((*funcs->find)(l, id, req_length))
		return;
	if (unlikely(filesize > MAX_CACHE_SIZE))
		return;
	if ((*save_func)(l, id, funcs)) {
#ifdef DEBUG_FAST
		fprintf(stderr, "[generic] %s Going to save file #%d (%uB, cached %luB)\n", funcs->algo_name, id, filesize, (*funcs->size)(l));
#endif
		while ((*funcs->size)(l) + filesize > MAX_CACHE_SIZE) {
#ifdef DEBUG_FAST
			int poped = (*funcs->pop)(l);
			fprintf(stderr, "[generic] %s Poped file #%d, current cached %luB)\n", funcs->algo_name, poped, (*funcs->size)(l));
#else
			(*funcs->pop)(l);
#endif
		}
		(*funcs->push)(l, id, filesize);
	}
}

// We can not allocate enough mem for all of these caches
//void* caches[ARRAY_COUNT(cache_funcs)][ARRAY_COUNT(cache_save_funcs)];
void* caches[ARRAY_COUNT(cache_funcs)];
int current_pass;

void init_cache(int size, int pass) {
	 int i,j;
	 current_pass = pass;
	 for (i=0; i<ARRAY_COUNT(cache_funcs); i++)
	 	caches[i] = (void*)((*cache_funcs[i].init)(size));
//		for (j=0; j<ARRAY_COUNT(cache_save_funcs); j++)
//	 		caches[i][j] = (void*)((*cache_funcs[i].init)(size));
}
EXPORT_SYMBOL(init_cache)

void update_cache(int id, UINT filesize, UINT req_length) {
	int i,j;
	for (i=0; i<ARRAY_COUNT(cache_funcs); i++)
		generic_cache(caches[i], id, filesize, req_length, &cache_funcs[i], cache_save_funcs[current_pass]);
//		for (j=0; j<ARRAY_COUNT(cache_save_funcs); j++)
//			generic_cache(caches[i][j], id, filesize, req_length, &cache_funcs[i], cache_save_funcs[j]);
}
EXPORT_SYMBOL(update_cache)

void __print_cache_utilization(FILE* fd, int num, ULONG total_reqs, ULONG total_traffic, void* cache,
	const cache_func_entry* funcs, const char* save_func_name) {

	ULONG reqs = (*funcs->req)(cache, num);
	ULONG traffic = (*funcs->traffic)(cache, num);
	fprintf(fd, "%s\t%s:\t", funcs->algo_name, save_func_name);
	fprintf(fd, "reqs %lu (%.2lf%%),\t", reqs, (double)reqs / (double)total_reqs * 100);
	fprintf(fd, "traffic %.2lf GB (%.2lf%%)\n", (double)traffic / (double)(1<<30), (double)traffic / (double)total_traffic * 100);
}

void print_cache_utilization(FILE* fd, int num, ULONG total_reqs, ULONG total_traffic) {
	int i,j;
	fprintf(fd, "\n");
	for (i=0; i<ARRAY_COUNT(cache_funcs); i++)
		__print_cache_utilization(fd, num, total_reqs, total_traffic, caches[i],
			&cache_funcs[i], cache_save_funcs_name[current_pass]);
//		for (j=0; j<ARRAY_COUNT(cache_save_funcs); j++) {
//			__print_cache_utilization(fd, num, total_reqs, total_traffic, caches[i][j],
//				&cache_funcs[i], cache_save_funcs_name[j]);
//		}
}
EXPORT_SYMBOL(print_cache_utilization)

void free_cache() {
	int i,j;
	for (i=0; i<ARRAY_COUNT(cache_funcs); i++)
		if (caches[i] != NULL)
			(*cache_funcs[i].free)(caches[i]);
//		for (j=0; j<ARRAY_COUNT(cache_save_funcs); j++)
//			if (caches[i][j] != NULL)
//				(*cache_funcs[i].free)(caches[i][j]);
}
EXPORT_SYMBOL(free_cache)
