#ifndef __CACHE_H__
#define __CACHE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "./container.h"
#include "./config.h"


#define CA_EXIT_SUCCESS 0
#define CA_EXIT_FAILURE 1

#define OFFSET_MASK ( UINT64_MAX >> ( 64 - B_OFFSET ) )
#define TAG_MASK    ( UINT64_MAX << ( 64 - B_TAG ) )
#define INDEX_MASK  ( UINT64_MAX & (!OFFSET MASK) & (!TAG_MASK) )

#define INDEX(x) ( ( x & INDEX_MASK ) >> B_OFFSET )
#define TAG(x)   ( ( x & TAG_MASK ) >> ( 64 - B_TAG) )


struct cache_entry {
    uint64_t tag;
    bool is_valid;
    bool is_dirty;

    char data[BLOCK_SIZE];

    struct lru_node *lru_ptr;
};

struct cache_way {
    struct cache_entry *tbl;
}

struct cache_set {
    struct cache_entry **ptr;
    struct lru_list *lru;
}

#endif
