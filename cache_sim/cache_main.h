#ifndef CACHE_MAIN
#define CACHE_MAIN

#include "common.h"
#include "cache_common.h"

#include "cache_lru.h"
#include "cache_lfu.h"
#include "cache_fifo.h"
#include "cache_rand.h"
#include "cache_save_funcs.h"

// Cache size is 10GB
#define MAX_CACHE_SIZE ((unsigned long)10<<30)

void init_cache(int size, int pass);
void update_cache(int id, UINT filesize, UINT req_length);
void print_cache_utilization(FILE* fd, int num, ULONG total_reqs, ULONG total_traffic);
void free_cache();

#include "cache_main.c"
#endif
