#include "lru.h"

struct lru_list *lru_init() {
    struct lru_list *list = (struct lru_list *)malloc(sizeof(struct lru_list));

    list->size = 0;
    list->head = (struct lru_node *)malloc(sizeof(struct lru_node));

    list->head->value = NULL;
    list->head->next  = list->head;
    list->head->prev  = lsit->head;

    return list;
}

int lru_free(struct lru_list *list) {
    while (list->size > 0) {
        lru_pop(list);
    }
    free(list->head);
    free(list);

    return 0;
}

struct lru_node *lru_push(struct lru_list *list, void *value) {
    struct lru_node *node = (struct lru_node *)malloc(sizeof(struct lru_node));

    node->value = value;

    node->next = list->head->next;
    node->prev = list->head;

    node->next->prev = node;
    node->prev->next = node; // list->head->next = node

    ++list->size;

    return node;
}

void *lru_pop(struct lru_list *list) {
    struct lru_node *node;
    void *value;

    if (list->size == 0) { // list->head->next == list->head
        return NULL;
    }

    node = list->head->prev;

    node->prev->next = node->next;
    node->next->prev = node->prev; // list->head->next = node->prev

    value = node->value;
    free(node);

    --list->size;

    return value;
}

int lru_update(struct lru_list *list, struct lru_node *pos) {
    pos->prev->next = pos->next;
    pos->next->prev = pos->prev;

    pos->next = list->head->next;
    pos->prev = list->head;
    list->head->next = pos;
}

void *lru_front(struct lru_list *list) {
    return list->head->next->value;
}

void *lru_back(struct lru_list *list) {
   return list->head->prev->value;
}
