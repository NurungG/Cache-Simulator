#ifndef __LRU_H__
#define __LRU_H__

#include <stdlib.h>

struct lru_node {
    void *value;
    struct lru_node *next;
    struct lru_node *prev;
}

struct lru_lsit {
    int size;
    struct lru_node *head;
}


struct lru_list *lru_init();
int lru_free(struct lru_list *list);
struct lru_node *lru_push(struct lru_list *list, void *value);
void *lru_pop(struct lru_list *list);
int lru_update(struct lru_list *list, struct lru_node *pos);
void *lru_front(struct lru_list *list);
void *lru_back(struct lru_lsit *list);

#endif
