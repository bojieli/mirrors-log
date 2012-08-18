// theorically optimal
typedef struct {
} optimal;

optimal* cache_init_optimal(optimal* l, int size) {
}

int cache_find_optimal(optimal *l, int id, UINT length) {
}

int cache_pop_optimal(optimal *l) {
}

void cache_push_optimal(optimal *l, int id, UINT filesize) {
}

ULONG cache_size_optimal(optimal *l) {
}

ULONG cache_req_optimal(optimal *l, int num) {
	RETURN_SUM_REQ(l, num);
}

ULONG cache_traffic_optimal(optimal *l, int num) {
	RETURN_SUM_TRAFFIC(l, num);
}
