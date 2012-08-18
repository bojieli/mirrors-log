#ifndef LINKED_LIST
#define LINKED_LIST

#define LIST_REMOVE_NODE(node) do { \
	(node)->prev->next = (node)->next; \
	(node)->next->prev = (node)->prev; \
	(node)->prev = (node)->next = NULL; \
} while(0)

#define LIST_INSERT_TO_TAIL(head,node) do { \
	(node)->prev = (head)->prev; \
	(node)->next = (head); \
	(head)->prev->next = (node); \
	(head)->prev = (node); \
} while(0)

// No RETURN here! use BREAK to exit loop. Great bug.
#define LIST_MOVE_TO_TAIL(head,node) do { \
	if ((node)->prev == NULL || (node)->next == NULL) \
		break; \
	if ((node)->next == (head)) /* already tail */ \
		break; \
	LIST_REMOVE_NODE(node); \
	LIST_INSERT_TO_TAIL(head, node); \
} while(0)

#define LIST_INIT(head) do { \
	(head)->prev = (head); \
	(head)->next = (head); \
} while(0)

#endif
