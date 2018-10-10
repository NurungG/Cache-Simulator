#ifndef __CACHE_H__
#define __CACHE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "./lru.h"


#define CA_EXIT_SUCCESS 0
#define CA_EXIT_FAILURE 1

#define INDEX(x) ( ( x & INDEX_MASK ) >> B_OFFSET )
#define TAG(x)   ( ( x & TAG_MASK ) >> ( 64 - B_TAG) )


struct cache_entry {
    uint64_t tag;
    bool is_valid;
    bool is_dirty;

    //char data[BLOCK_SIZE];

    struct lru_node *lru_ptr;
};

struct cache_way {
    struct cache_entry *tbl;
};

struct cache_set {
    struct cache_entry **ptr;
    struct lru_list *lru;
};

struct cache_info {
    int (*create) ();
    int (*destroy) ();
    int (*read) ();
    int (*write) ();

    int capacity;
    int n_way;
    int block_sz;

    int n_entry;
    int n_set;

    int b_offset;
    int b_index;
    int b_tag;

    uint64_t offset_mask;
    uint64_t tag_mask;
    uint64_t index_mask;

    struct cache_way *way;
    struct cache_set *set;

    uint64_t total_cnt;
    uint64_t read_cnt;
    uint64_t write_cnt;

    uint64_t read_miss;
    uint64_t write_miss;

    uint64_t clean_evict;
    uint64_t dirty_evict;

    uint64_t checksum;
};


int cache_create();
int cache_destroy();
int cache_read(uint64_t addr);
int cache_write(uint64_t addr);

#endif
