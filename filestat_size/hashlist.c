// A binary-safe hash list with little memory usage
// (C) 2012 Bojie Li <bojieli@gmail.com>
#include "common.h"
#include "hashfunc.h"

#ifdef DEBUG
int collision_counter;
int repeat_counter;
int notfound_counter;
int found_counter;
#endif

// The 'check hash' approach used by Blizzard: check 2 additional hashes
// instead of comparing the string. This saves substantial memory, while
// there is a 10^(-22) probability of getting wrong answer.

// Use unsigned INT instead of LONG for storage since memory is precious.
// Processing speed decreases on 64-bit machines.
typedef struct {
	UINT hash1;
	UINT hash2;
	UINT counter;
	ULONG total;
} slot, *list;

int find_str(list l, ULONG slot_num, char *str, ULONG length) {
	ULONG pos = first_hash(str, length) % slot_num;
	UINT start = pos;
	UINT check1, check2;
	if (l[pos].counter == 0)
		return -1;
	check1 = check_hash1(str, length);
	check2 = check_hash2(str, length);
	do {
		if (l[pos].hash1 == check1 && l[pos].hash2 == check2)
		    	return pos;
		pos++;
		if (unlikely(pos == slot_num))
			pos = 0;
		if (l[pos].counter == 0)
			return -1;
	} while(start != pos);
	return -1;
}

STATUS add_str(list l, ULONG slot_num, char *str, ULONG length) {
	ULONG pos = first_hash(str, length) % slot_num;
	ULONG start = pos;
	UINT check1 = check_hash1(str, length);
	UINT check2 = check_hash2(str, length);
	do {
		if (l[pos].counter == 0) {
			l[pos].hash1 = check1;
			l[pos].hash2 = check2;
			l[pos].counter = 1;
			return 1;
		}
		if (unlikely(l[pos].hash1 == check1 && l[pos].hash2 == check2)) {
			l[pos].counter++;
#ifdef DEBUG
			repeat_counter++;
#endif
			return 1;
		}
#ifdef DEBUG
		collision_counter++;
#endif
		pos++;
		if (unlikely(pos == slot_num))
			pos = 0;
	} while(start != pos);
	return 0;
}

list init_list(ULONG slot_num) {
	list l = (void *)malloc(slot_num * sizeof(slot));
	if (l == NULL)
		return NULL;
#ifdef DEBUG
	fprintf(stderr, "slot_num = %d, sizeof slot = %d, mem = %d MB\n", slot_num, sizeof(slot), slot_num * sizeof(slot) / 1024 / 1024);
	collision_counter = 0;
	repeat_counter = 0;
	notfound_counter = 0;
	found_counter = 0;
#endif
	bzero(l, slot_num * sizeof(slot));
	return l;
}

STATUS add_counter(list l, ULONG slot_num, char *str, ULONG length, ULONG total_inc) {
	ULONG pos = find_str(l, slot_num, str, length);
	if (unlikely(pos == -1)) {
#ifdef DEBUG
//		fprintf(stderr, "\nNot found: ");
//		fwrite(str, 1, length, stderr);
//		fprintf(stderr, "\n");
		notfound_counter++;
#endif
		return 0;
	}
#ifdef DEBUG
	found_counter++;
#endif
	l[pos].counter++;
	l[pos].total += total_inc;
	return 1;
}

ULONG get_counter(list l, ULONG slot_num, char *str, ULONG length, ULONG* total) {
	ULONG pos = find_str(l, slot_num, str, length);
	if (unlikely(pos == -1)) {
		return 0;
	}
	*total = l[pos].total;
	return l[pos].counter;
}

#ifdef DEBUG
int occupied_slots_num(list l, ULONG slot_num) {
	int i, occupied = 0;
	for (i=0; i<slot_num; i++) {
		if (l[i].counter > 0)
			occupied++;
	}
	return occupied;
}

int hash_collision_count() {
	return collision_counter;
}

int repeat_str_count() {
	return repeat_counter;
}

int notfound_str_count() {
	return notfound_counter;
}

int found_str_count() {
	return found_counter;
}
#endif
