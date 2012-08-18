// Note: id ranges from 1 to num, index 0 acts as sentinel

#include "common.h"

#define RETURN_SUM_REQ(cache,num) do { \
	ULONG reqs = 0; \
	int i; \
	for (i=1; i<=(num); i++) \
		reqs += cache->l[i].req; \
	return reqs; \
} while(0)

#define RETURN_SUM_TRAFFIC(cache,num) do { \
	ULONG traffic = 0; \
	int i; \
	for (i=1; i<=(num); i++) \
		traffic += cache->l[i].traffic; \
	return traffic; \
} while(0)

#define RETURN_STR(str) do { \
	const char* retval = str; \
	return retval; \
} while(0)

#ifdef DEBUGMEM
#define PRINT_MEMINIT(each,num) do { \
	fprintf(stderr, "MEM %d*%d = %d\n", (each), (num), (each)*(num)); \
} while(0)
#else
#define PRINT_MEMINIT
#endif

// Note: cache variable will be used then
#define CACHE_INIT(size,listtype,nodetype) \
	listtype* cache = (listtype *)malloc(sizeof(listtype)); \
	if (unlikely(cache == NULL)) \
		return NULL; \
	bzero(cache, sizeof(listtype)); \
	cache->l = (nodetype *)malloc(sizeof(nodetype) * (size+1)); \
	if (unlikely(cache->l == NULL)) { \
		free(cache); \
		return NULL; \
	} \
	bzero(cache->l, sizeof(nodetype) * (size+1)); \
	PRINT_MEMINIT(sizeof(nodetype), size+1);

#define CACHE_FREE(cache) do { \
	if (cache != NULL) { \
		if (cache->l != NULL) \
			free(cache->l); \
		free(cache); \
	} \
} while(0)

#define RETURN_CACHE_FIND(cache,id,length) do { \
	cache->l[id].hits++; \
	if (cache->l[id].cached) { \
		CACHE_HIT_HOOK(cache, id); \
		cache->l[id].req++; \
		cache->l[id].traffic += length; \
		return true; \
	} \
	return false; \
} while(0)

#define RETURN_CACHE_HITS(cache,id) do { \
	return cache->l[id].hits; \
} while(0)

#define RETURN_CACHE_SIZE(cache) do { \
	return cache->cache_size; \
} while(0)

#define RETURN_POP_CACHE(cache,poped,nodesize) do { \
	poped->cached = false; \
	poped->hits = 0; \
	cache->cache_size -= poped->filesize; \
	return (poped - cache->l) / (nodesize); \
} while(0)

#define PUSH_CACHE(cache,pushed,filesize) do { \
	pushed->cached = true; \
	pushed->hits = 0; \
	pushed->filesize = filesize; \
	cache->cache_size += filesize; \
} while(0)
