#ifndef __PSEUDO_LRU_H__
#define __PSEUDO_LRU_H__

#include <stdlib.h>
#include <stdint.h>

struct lru_node {
    void *value;
    uint8_t idx;
};

struct lru_list {
    int max;
    int size;
    struct lru_node *node_arr;
    uint16_t lru_bit;
    int num_bits;
};


struct lru_list *lru_init(int max_sz);
int lru_free(struct lru_list *list);
struct lru_node *lru_push(struct lru_list *list, void *value);
int lru_update(struct lru_list *list, struct lru_node *pos);
void *lru_get_victim(struct lru_list *list);

#endif
