#include "cache.h"

struct cache_info cache = {
    create  = cache_create,
    destroy = cache_destroy,
    read    = cache_read,
    write   = cache_write
};

int cache_create() {
    cache.way = (struct cache_way *)malloc(sizeof(struct cache_way) * N_WAY);
    cache.set = (struct cache_set *)malloc(sizeof(struct cache_set) * N_SET);

    for (int i = 0; i < N_WAY; i++) {
        cache.way[i].tbl =
                (struct cache_entry *)malloc(sizeof(struct cache_entry) * N_SET);
    }
    for (int i = 0; i < N_SET; i++) {
        cache.set[i].ptr =
                (struct cache_entry **)malloc(sizeof(struct cache_entry *) * N_WAY);
        cache.set[i].lru = lru_init();

        for (int j = N_WAY; j >= 0; j--) {
            cache.set[i].ptr[j] = &cache.way[j].tbl[i];
            cache.set[i].ptr[j]->is_valid = false;
            cache.set[i].ptr[j]->lru_ptr =
                    lru_push(cache.set[i].lru, (void *)cache.set[i].ptr[j]);
        }
    }

    return CA_EXIT_SUCCESS;
}

int cache_destroy() {
    for (int i = 0; i < N_WAY; i++) {
        free(cache.way[i]);
    }
    for (int i = 0; i < N_SET; i++) {
        lru_free(cache.set[i].lru);
        free(cache.set[i]);
    }
    free(cache.way);
    free(cache.set);

    return CA_EXIT_SUCCESS;
}

int cache_read(uint64_t addr) {
    struct cache_set *set;
    uint64_t tag;
    struct cache_entry *iter;

    struct cache_entry *update, *victim;

    bool is_hit = false;

    set = &cache.set[INDEX(addr)];
    tag = TAG(addr);

    // Check cache hit or miss
    for (int i = 0; i < N_WAY; i++) {
        iter = set->ptr[i];

        if ( iter->is_valid && (tag == iter->tag) ) {
            is_hit = true;
            update = iter;
            break;
        }
    }

    // Update
    if (!is_hit) {
        victim = lru_front(set->lru);
        if (victim->is_valid && victim->is_dirty) {
            // Write-back
        }

        // Read data
        victim->tag      = tag;
        victim->is_valid = true;
        victim->is_dirty = false;
        lru_update(set->lru, victim->lru_ptr);

    } else {
        lru_update(set->lru, update->lru_ptr);
    }

    return CA_EXIT_SUCCESS;
}

int cache_write(uint64_t addr) {
    struct cache_set *set;
    uint64_t tag;
    struct cache_entry *iter;

    return CA_EXIT_SUCCESS;
}
