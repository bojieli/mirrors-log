#include <fcntl.h>
#include "hashlist.c"
#include "mysqlclient.c"
#include "common.h"
#include "cache_main.h"

#define DBHOST "localhost"
#define DBUSER "root"
#define DBPASS ""
#define DBNAME "mirrors"
#define DBSOCK "/tmp/mysql-ib.sock"

// return string length, -1 for error
inline int get_next_url(char* buf, unsigned long* req_len) {
	MYSQL_ROW row;
	MYSQL_RES *res;
	if (!(res = buffered_get_next_row(&row)))
		return -1;
	unsigned long *lengths = mysql_fetch_lengths(res);
	memcpy(buf, row[0], lengths[0]);
	if (req_len != NULL)
		*req_len = row[1] != NULL ? strtol(row[1], NULL, 10) : 0;
	return lengths[0]; // note that length can be zero
}

inline void init_get_next_url(MYSQL* sock, char* table_name) {
	char *query;
	if (!strcmp(table_name, "log"))
		query = nsprintf("SELECT url, length FROM %s", table_name);
	else
		query = nsprintf("SELECT url, filesize FROM %s", table_name);
	init_buffered_get_next_row(sock, query);
	free(query);
}

int main(int argc, char** argv) {

	MYSQL mysql;
	MYSQL *sock = mysql_connect_db(&mysql, DBHOST, DBUSER, DBPASS, DBNAME, DBSOCK);
	if (sock == NULL)
		exit(1);

	int file_num = mysql_result_int(sock, "SELECT COUNT(*) FROM files");
	if (file_num <= 0) {
		fprintf(stderr, "Abnormal row num of files: %d\n", file_num);
		exit(1);
	}
#ifdef DEBUG
	fprintf(stderr, "\nTable file Rows: %d\n", file_num);
#endif
#ifdef DEBUGTIME
	int start_time = (int)time(NULL);
	fprintf(stderr, "Start loading files: epoch %d\n", start_time);
#endif
	// we hope the hashlist will have not too much collision
	int slot_num = file_num * 3;
	list l = init_list(slot_num);
	if (l == NULL) {
		fprintf(stderr, "Error creating hashlist with %d slots\n", slot_num);
		exit(1);
	}

	init_get_next_url(sock, "files");
	char url[512];
	int length;
	unsigned long filesize;
#ifdef DEBUG_FAST
	int count = 0;
#endif
	while (0 <= (length = get_next_url(url, &filesize))) {
		if (length > 0) {
			int slot = add_str(l, slot_num, url, length);
			if (unlikely(slot < 0)) {
				fprintf(stderr, "Error %d inserting hashlist: %s\n", slot, url);
				exit(1);
			}
			set_size(l, slot, filesize);
#ifdef DEBUG_FAST
	//		if (++count > (1<<20))
	//			break;
#endif
		}
	}

#ifdef DEBUG
	fprintf(stderr, "\n%d slots occupied after loading files\n", occupied_slots_num(l, slot_num));
	fprintf(stderr, "Hash collision %d times\n", hash_collision_count());
	fprintf(stderr, "Repeat string count: %d\n", repeat_str_count());
#endif

// We do NOT have enough memory to do all experiments at once
int pass = 0;
while (pass < ARRAY_COUNT(cache_save_funcs)) {

#ifdef DEBUGTIME
	fprintf(stderr, "Start loading log @%ds\n", (int)time(NULL) - start_time);
#endif
#ifdef DEBUG_FAST
	count = 0;
#endif
	init_cache(file_num, pass);
	init_get_next_url(sock, "log");
	unsigned long req_len;
	while (0 <= (length = get_next_url(url, &req_len))) {
		if (length > 0) {
			int slot = add_counter(l, slot_num, url, length, req_len);
			if (slot >= 0) // found url
				update_cache(get_id(l, slot), get_size(l, slot), req_len);
#ifdef DEBUG_FAST
			if (++count > (1<<20))
				break;
#endif
		}
	}

#ifdef DEBUG
	fprintf(stderr, "\nNot found strings count: %d\n", notfound_str_count());
	fprintf(stderr, "\nFound strings count: %d\n", found_str_count());
#endif
#ifdef DEBUGTIME
	fprintf(stderr, "Start collecting cache info @%ds\n", (int)time(NULL) - start_time);
#endif
	
	unsigned long total_reqs = mysql_result_int(sock, "SELECT SUM(requests) FROM filestat");
	unsigned long total_traffic = mysql_result_int(sock, "SELECT SUM(traffic) FROM filestat");
	if (pass == 0) // only output once global info
		fprintf(stdout, "Total %d files, %lu reqs, %.2lf GB traffic\n", file_num, total_reqs, (double)total_traffic / (double)(1<<30));
	print_cache_utilization(stdout, file_num, total_reqs, total_traffic);
	free_cache();

#ifdef DEBUGTIME
	fprintf(stderr, "\nEND @%ds\n", (int)time(NULL) - start_time);
#endif
	pass++;
} // end pass

	mysql_close(&mysql);
	return 0;
}
