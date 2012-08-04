// A simple and naive mysql client library including rows buffering
// Note that MySQL query strings are not binary-safe.
// (C) 2012 Bojie Li <bojieli@gmail.com>
#include "common.h"
// Remember to gcc -I -L mysql lib
#include "mysql.h"

// This is easier to use, but keep an eye on memory leak!
#include <stdarg.h>
#define MAX_STRLEN 256
char* nsprintf(char* format, ...) {
	va_list arg;
	va_start(arg, format);
	char *buf = malloc(MAX_STRLEN);
	vsprintf(buf, format, arg);
	va_end(arg);
	return buf;
}
#undef MAX_STRLEN

MYSQL* mysql_connect_db(MYSQL* mysql, const char* host, const char* dbuser, const char* dbpass, const char* dbname, const char* dbsock) {
	MYSQL* sock;
	mysql_init(mysql);
	if (!(sock = mysql_real_connect(mysql, host, dbuser, dbpass, dbname, 0, dbsock, 0))) {
		fprintf(stderr, "Couldn't connect to MySQL engine!\n%s\n", mysql_error(mysql));
		return NULL;
	}
	return sock;
}
// Remember to do:
// mysql_close(mysql);

MYSQL_RES* do_mysql_query(MYSQL* sock, char* query) {
	MYSQL_RES *res;
	if (unlikely(mysql_query(sock, query))) {
		fprintf(stderr, "\nQuery failed (%s)\n", mysql_error(sock));
		return NULL;
	}
	if (unlikely(!(res = mysql_store_result(sock)))) {
		fprintf(stderr, "\nCouldn't get result from %s\n", mysql_error(sock));
		return NULL;
	}
	return res;
}
// Remember to do:
// mysql_free_result(MYSQL_RES *result);

int mysql_result(MYSQL* sock, char* query, char** result) {
	MYSQL_RES *res = do_mysql_query(sock, query);
	MYSQL_ROW row = mysql_fetch_row(res);
	int length;
	if (unlikely(row[0] == NULL || !(length = strlen(row[0]))))
		return 0;
	*result = malloc(length + 1);
	memcpy(*result, row[0], length + 1);
	mysql_free_result(res);
	return 1;
}

int mysql_result_int(MYSQL* sock, char* query) {
	int res_int;
	char* res;
	if (unlikely(!mysql_result(sock, query, &res)))
		return 0;
	res_int = atoi(res);
	free(res);
	return res_int;
}

#define BUFFER_SIZE 65536

MYSQL_RES* do_buffered_get_next_row(MYSQL* new_sock, char* new_query, MYSQL_ROW* row) {
	static unsigned buffered = 0;
	static unsigned start = 0;
	static char* query = NULL;
	static MYSQL* sock;
	static MYSQL_RES *res = NULL;
	
	if (unlikely(new_query)) {
		buffered = start = 0;
		sock = new_sock;
		if (query)
			free(query);
		query = malloc(strlen(new_query) + 1);
		strcpy(query, new_query);
#ifdef DEBUG
		fprintf(stderr, "\nQuery: %s\n", query);
#endif
		return NULL;
	}

	if (unlikely(buffered == 0)) {
		if (res)
			mysql_free_result(res);
		char *full_query = nsprintf("%s LIMIT %d,%d", query, start, BUFFER_SIZE);
		res = do_mysql_query(sock, full_query);
		free(full_query);
		if (!res)
			return NULL;
		if (!(buffered = mysql_num_rows(res)))
			return NULL;
		start += buffered;
#ifdef DEBUG
		fprintf(stderr, "%d\t", start);
#endif
	}
	buffered--;
	*row = mysql_fetch_row(res);
	return res;
}

inline void init_buffered_get_next_row(MYSQL* sock, char* query) {
	do_buffered_get_next_row(sock, query, NULL);
}

inline MYSQL_RES* buffered_get_next_row(MYSQL_ROW* row) {
	return do_buffered_get_next_row(NULL, NULL, row);
}
