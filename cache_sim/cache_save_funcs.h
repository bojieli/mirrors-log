#ifndef CACHE_SAVE_FUNCS
#define CACHE_SAVE_FUNCS

#define CACHE_SAVE_FUNC_PROTO(name) bool name(void*, int, cache_func_entry*)

#include "cache_common.h"

bool cache_save_every(void* l, int id, cache_func_entry* funcs) {
	return true;
}

bool cache_save_2hits(void* l, int id, cache_func_entry* funcs) {
	if (funcs->hits(l, id) >= 2)
		return true;
	return false;
}

bool cache_save_3hits(void* l, int id, cache_func_entry* funcs) {
	if (funcs->hits(l, id) >= 3)
		return true;
	return false;
}

bool cache_save_10hits(void* l, int id, cache_func_entry* funcs) {
	if (funcs->hits(l, id) >= 10)
		return true;
	return false;
}

bool (*cache_save_funcs[])(void*, int, cache_func_entry*) = {
	cache_save_every,
	cache_save_2hits,
	cache_save_3hits,
	cache_save_10hits
};

const char* cache_save_funcs_name[] = {
	"saveEvery", "save2Hits", "save3Hits", "save10Hits"
};

#endif
