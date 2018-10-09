#include "pseudo_lru.h"

static int num_of_bits(int num) {
    int ret = 0;
    while (num >>= 1) {
        ++ret;
    }
    return ret;
}

struct lru_list *lru_init(int max_sz) {
    struct lru_list *list = (struct lru_list *)malloc(sizeof(struct lru_list));

    list->max      = max_sz;
    list->size     = 0;
    list->node_arr = (struct lru_node *)malloc(sizeof(struct lru_node) * max_sz);
    list->lru_bit  = 0;
    list->num_bits = num_of_bits(max_sz);

    for (int i = 0; i < max_sz; i++) {
        list->node_arr[i].idx = i;
    }

    return list;
}

int lru_free(struct lru_list *list) {
    free(list->node_arr);
    free(list);
    return 0;
}

struct lru_node *lru_push(struct lru_list *list, void *value) {
    list->node_arr[list->max-list->size-1].value = value;
    list->size++;
    return &list->node_arr[list->max-list->size-2];
}

int lru_update(struct lru_list *list, struct lru_node *node) {
    int idx = node->idx;
    int pos = 1;

    for (int i = 1; i <= list->num_bits; i++) {
        if ((idx & (1 << (list->num_bits-i))) == 0) {
            if (( list->lru_bit & (1 << pos) ) == 0) {
                list->lru_bit |= 1 << pos;
            }
            pos = pos*2;
        } else {
            if ( (list->lru_bit & (1 << pos) ) != 0) {
                list->lru_bit &= -1 ^ (1 << pos);
            }
            pos = pos*2 + 1;
        }
    }
    return 0;
}

void *lru_get_victim(struct lru_list *list) {
    int pos = 1;
    for (int i = 0; i < list->num_bits; i++) {
        if ( (list->lru_bit & (1 << pos) ) == 0) {
            // go up
            pos = pos*2;
        } else {
            // go down
            pos = pos*2 + 1;
        }
    }
    return list->node_arr[pos-list->max].value;
}
