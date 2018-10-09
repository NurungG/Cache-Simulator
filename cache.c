#include "cache.h"

struct basic_cache cache = {
    .create  = cache_create,
    .destroy = cache_destroy,
    .read    = cache_read,
    .write   = cache_write
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

        for (int j = N_WAY-1; j >= 0; j--) {
            cache.set[i].ptr[j] = &cache.way[j].tbl[i];
            cache.set[i].ptr[j]->is_valid = false;
            cache.set[i].ptr[j]->lru_ptr =
                    lru_push(cache.set[i].lru, (void *)cache.set[i].ptr[j]);
        }
    }

    return CA_EXIT_SUCCESS;
}

int cache_destroy() {
    int i, j;

    cache.total_cnt = cache.read_cnt + cache.write_cnt;

    for (i = 0; i < N_WAY; i++) {
        for (j = 0; j < N_SET; j++) {
            struct cache_entry *entry = &cache.way[i].tbl[j];
            if (entry->is_valid) {
                cache.checksum ^= (entry->tag ^ j << 1) | entry->is_dirty;
            }
        }
    }
    // Print info
    puts("-- General Stats --");
    printf("Capacity: %d\n", 1 << CAPACITY_BITS);
    printf("Way: %d\n", N_WAY);
    printf("Block size: %d\n", BLOCK_SIZE);

    printf("Total accesses: %lu\n", cache.total_cnt);
    printf("Read accesses: %lu\n", cache.read_cnt);
    printf("Write accesses: %lu\n", cache.write_cnt);

    printf("Read misses: %lu\n", cache.read_miss);
    printf("Write misses: %lu\n", cache.write_miss);
    printf("Read miss rate: %f%%\n", (float)cache.read_miss / cache.read_cnt * 100);
    printf("Write miss rate: %f%%\n", (float)cache.write_miss / cache.write_cnt * 100);

    printf("Clean evictions: %lu\n", cache.clean_evict);
    printf("Dirty evictions: %lu\n", cache.dirty_evict);

    printf("Checksum: 0x%lx\n", cache.checksum);

    for (int i = 0; i < N_WAY; i++) {
        free(cache.way[i].tbl);
    }
    for (int i = 0; i < N_SET; i++) {
        lru_free(cache.set[i].lru);
    }
    free(cache.way);
    free(cache.set);

    return CA_EXIT_SUCCESS;
}

int cache_read(uint64_t addr) {
    struct cache_set *set;
    uint64_t tag;

    struct cache_entry *iter;
    struct cache_entry *target;

    bool is_hit = false;

    ++cache.read_cnt;

    set = &cache.set[INDEX(addr)];
    tag = TAG(addr);

    // Check cache hit or miss
    for (int i = 0; i < N_WAY; i++) {
        iter = set->ptr[i];

        if ( iter->is_valid && (tag == iter->tag) ) {
            is_hit = true;
            target = iter;
            break;
        }
    }

    if (!is_hit) {
        ++cache.read_miss;

        target = lru_get_victim(set->lru);
        if (target->is_valid && target->is_dirty) {
            ++cache.dirty_evict;

            // Write-back
            // target->data => memory
        } else {
            ++cache.clean_evict;
        }

        // Read data
        target->tag      = tag;
        target->is_valid = true;
        target->is_dirty = false;

        // target->data <= memory
    }

    // Update
    lru_update(set->lru, target->lru_ptr);

    return CA_EXIT_SUCCESS;
}

int cache_write(uint64_t addr) {
    struct cache_set *set;
    uint64_t tag;

    struct cache_entry *iter;
    struct cache_entry *target;

    bool is_hit = false;

    ++cache.write_cnt;

    set = &cache.set[INDEX(addr)];
    tag = TAG(addr);

    // Check cache hit or miss
    for (int i = 0; i < N_WAY; i++) {
        iter = set->ptr[i];

        if ( iter->is_valid && (tag == iter->tag) ) {
            is_hit = true;
            target = iter;
            break;
        }
    }

    if (!is_hit) {
        ++cache.write_miss;

        target = lru_get_victim(set->lru);
        if (target->is_valid && target->is_dirty) {
            ++cache.dirty_evict;

            // Write-back
            // target->data => memory
        } else {
            ++cache.clean_evict;
        }

        // Read data
        target->tag      = tag;
        target->is_valid = true;

        // target->data <= memory
    }

    // Update
    lru_update(set->lru, target->lru_ptr);

    // Mark as dirty
    target->is_dirty = true;

    return CA_EXIT_SUCCESS;
}
