#include <fcntl.h>
#include "hashlist.c"
#include "mysqlclient.c"
#include "common.h"

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
		*req_len = atoi(row[1]);
	return lengths[0]; // note that length can be zero
}

inline void init_get_next_url(MYSQL* sock, char* table_name) {
	char *query;
	if (!strcmp(table_name, "log"))
		query = nsprintf("SELECT url, length FROM %s", table_name);
	else
		query = nsprintf("SELECT url FROM %s", table_name);
	init_buffered_get_next_row(sock, query);
	free(query);
}

// return length
unsigned array_implode(char** line, unsigned fields_num, MYSQL_ROW row, unsigned long* lengths, unsigned long newfield1, unsigned long newfield2) {
	// field1\tfield2\t...fieldn\tnewfield\n
	char *newfield1_s = nsprintf("%lu\t", newfield1);
	char *newfield2_s = nsprintf("%lu\n", newfield2);
	unsigned len = strlen(newfield1_s) + strlen(newfield2_s);
	unsigned i;
	for (i=0; i<fields_num; i++) {
		len += lengths[i] + 1;
	}
	char* lp = malloc(len + 1); // the last byte for \0
	*line = lp;
	*(lp+len) = '\0';
	for (i=0; i<fields_num; i++) {
		memcpy(lp, row[i], lengths[i]);
		lp += lengths[i];
		*(lp++) = '\t';
	}
	memcpy(lp, newfield1_s, strlen(newfield1_s));
	memcpy(lp + strlen(newfield1_s), newfield2_s, strlen(newfield2_s));
	free(newfield1_s);
	free(newfield2_s);
	return len;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: ./stat outputfile\n");
		exit(1);
	}
	int fd_out;
	if (!(fd_out = open(argv[1], O_WRONLY | O_CREAT, 0644))) {
		fprintf(stderr, "Cannot open output file!\n");
		exit(1);
	}

	MYSQL mysql;
	MYSQL *sock = mysql_connect_db(&mysql, DBHOST, DBUSER, DBPASS, DBNAME, DBSOCK);
	if (sock == NULL)
		exit(1);

	int rows_num = mysql_result_int(sock, "SELECT COUNT(*) FROM files");
	if (rows_num <= 0) {
		fprintf(stderr, "Abnormal row num of files: %d\n", rows_num);
		exit(1);
	}
#ifdef DEBUG
	fprintf(stderr, "\nTable file1 Rows: %d\n", rows_num);
#endif
#ifdef DEBUGTIME
	int start_time = time(NULL);
	fprintf(stderr, "Start loading files: epoch %d\n", start_time);
#endif
	// we hope the hashlist will have not too much collision
	int slot_num = rows_num * 3;
	list l = init_list(slot_num);
	if (l == NULL) {
		fprintf(stderr, "Error creating hashlist with %d slots\n", slot_num);
		exit(1);
	}

	init_get_next_url(sock, "files");
	char url[512];
	int length;
	while (0 <= (length = get_next_url(url, NULL))) {
		if (length > 0)
			add_str(l, slot_num, url, length);
	}

#ifdef DEBUG
	fprintf(stderr, "\n%d slots occupied after loading files\n", occupied_slots_num(l, slot_num));
	fprintf(stderr, "Hash collision %d times\n", hash_collision_count());
	fprintf(stderr, "Repeat string count: %d\n", repeat_str_count());
#endif
#ifdef DEBUGTIME
	fprintf(stderr, "Start loading log @%ds\n", time(NULL) - start_time);
#endif

	unsigned long req_len;
	init_get_next_url(sock, "log");
	while (0 <= (length = get_next_url(url, &req_len))) {
		if (length > 0)
			add_counter(l, slot_num, url, length, req_len);
	}

#ifdef DEBUG
	fprintf(stderr, "\nNot found strings count: %d\n", notfound_str_count());
	fprintf(stderr, "\nFound strings count: %d\n", found_str_count());
#endif
#ifdef DEBUGTIME
	fprintf(stderr, "Start loading files @%ds\n", time(NULL) - start_time);
#endif

	init_buffered_get_next_row(sock, "SELECT * FROM files");
	MYSQL_ROW file_fields;
	// Do not free() me! It contains many rows and is managed by buffer.
	MYSQL_RES* res;
	while (res = buffered_get_next_row(&file_fields)) {
		// the first field is id, second is url
		unsigned long *lengths = mysql_fetch_lengths(res);
		unsigned long total;
		unsigned long count = get_counter(l, slot_num, file_fields[1], lengths[1], &total);
		if (unlikely(count == 0)) {
			fprintf(stderr, "\nError: file hash not found: %s\n", file_fields[1]);
			continue;
		}
		char* line;
		// note that the counter includes one in table `files'
		unsigned len = array_implode(&line, mysql_num_fields(res), file_fields, lengths, count-1, total);
		write(fd_out, line, len);
		free(line);
	}

#ifdef DEBUGTIME
	fprintf(stderr, "\nEND @%ds\n", time(NULL) - start_time);
#endif
	mysql_close(&mysql);
	close(fd_out);
	return 0;
}
