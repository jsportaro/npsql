#ifndef __RSQL_LIST_H__
#define __RSQL_LIST_H__

typedef struct ListHead {
     struct ListHead *next, *prev;
} ListHead;

#define ListHeadInit(name) { &(name), &(name) }

#define ListHead(name) \
	struct list_head name = ListHeadInit(name)

static inline void InitListHead(ListHead *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __ListAdd(ListHead *new,
							  ListHead *prev,
							  ListHead *next)
{
	next->prev = new;
	new->next  = next;
	new->prev  = prev;
	prev->next = new;
}

static inline void ListAdd(ListHead *new, ListHead *head)
{
    __ListAdd(new, head, head->next);
}

static inline void ListAddTail(ListHead *new, ListHead *head)
{
	__ListAdd(new, head->prev, head);
}

#endif
