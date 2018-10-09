#include "cache.h"

struct cache_info cache = {
    .create  = cache_create,
    .destroy = cache_destroy,
    .read    = cache_read,
    .write   = cache_write
};

static int num_of_bits(int num) {
    int ret = 0;
    while ((num = num >> 1)) {
        ++ret;
    }
    return ret;
}

int cache_create(struct cache_info *ci, int _capacity, int _n_way, int _block_sz) {
    ci->capacity = _capacity * 1024;
    ci->n_way    = _n_way;
    ci->block_sz = _block_sz;

    ci->n_entry  = ci->capacity / ci->block_sz;
    ci->n_set    = ci->n_entry / ci->n_way;

    ci->b_offset = num_of_bits(ci->block_sz);
    ci->b_index  = num_of_bits(ci->capacity / ci->n_way / ci->block_sz);
    ci->b_tag    = 64 - ci->b_offset - ci->b_index;

    ci->offset_mask = UINT64_MAX >> ( 64 - ci->b_offset);
    ci->tag_mask    = UINT64_MAX << ( 64 - ci->b_tag);
    ci->index_mask  = ( UINT64_MAX ^ ci->offset_mask ) & (UINT64_MAX ^ ci->tag_mask);

    ci->way = (struct cache_way *)malloc(sizeof(struct cache_way) * ci->n_way);
    ci->set = (struct cache_set *)malloc(sizeof(struct cache_set) * ci->n_set);

    for (int i = 0; i < ci->n_way; i++) {
        ci->way[i].tbl =
                (struct cache_entry *)malloc(sizeof(struct cache_entry) * ci->n_set);
    }
    for (int i = 0; i < ci->n_set; i++) {
        ci->set[i].ptr =
                (struct cache_entry **)malloc(sizeof(struct cache_entry *) * ci->n_way);
        ci->set[i].lru = lru_init(ci->n_way);

        for (int j = ci->n_way-1; j >= 0; j--) {
            ci->set[i].ptr[j] = &ci->way[j].tbl[i];
            ci->set[i].ptr[j]->is_valid = false;
            ci->set[i].ptr[j]->lru_ptr =
                    lru_push(ci->set[i].lru, (void *)ci->set[i].ptr[j]);
        }
    }

    return CA_EXIT_SUCCESS;
}

int cache_destroy(struct cache_info *ci) {
    int i, j;

    // Calculate some info
    ci->total_cnt = ci->read_cnt + ci->write_cnt;

    for (i = 0; i < ci->n_way; i++) {
        for (j = 0; j < ci->n_set; j++) {
            struct cache_entry *entry = ci->set[j].ptr[i];
            if (entry->is_valid) {
                ci->checksum ^= ( ( entry->tag ^ j ) << 1 ) | entry->is_dirty;
            }
        }
    }

    // Print info
    puts("-- General Stats --");
    printf("Capacity: %d\n", ci->capacity / 1024);
    printf("Way: %d\n", ci->n_way);
    printf("Block size: %d\n", ci->block_sz);

    printf("Total accesses: %lu\n", ci->total_cnt);
    printf("Read accesses: %lu\n", ci->read_cnt);
    printf("Write accesses: %lu\n", ci->write_cnt);

    printf("Read misses: %lu\n", ci->read_miss);
    printf("Write misses: %lu\n", ci->write_miss);
    printf("Read miss rate: %.2f%%\n", (float)ci->read_miss / ci->read_cnt * 100);
    printf("Write miss rate: %.2f%%\n", (float)ci->write_miss / ci->write_cnt * 100);

    printf("Clean evictions: %lu\n", ci->clean_evict);
    printf("Dirty evictions: %lu\n", ci->dirty_evict);

    printf("Checksum: 0x%lx\n", ci->checksum);

    // Clear
    for (int i = 0; i < ci->n_way; i++) {
        free(ci->way[i].tbl);
    }
    for (int i = 0; i < ci->n_set; i++) {
        lru_free(ci->set[i].lru);
    }
    free(ci->way);
    free(ci->set);

    return CA_EXIT_SUCCESS;
}

static inline uint64_t get_index(uint64_t addr) {
    return ( ( addr & cache.index_mask ) >> cache.b_offset );
}

static inline uint64_t get_tag(uint64_t addr) {
    return ( ( addr & cache.tag_mask ) >> ( 64 - cache.b_tag ) );
}

int cache_read(uint64_t addr) {
    struct cache_set *set;
    uint64_t tag;

    struct cache_entry *iter;
    struct cache_entry *target;

    bool is_hit = false;

    ++cache.read_cnt;

    set = &cache.set[get_index(addr)];
    tag = get_tag(addr);

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
        } else if (target->is_valid){
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

    set = &cache.set[get_index(addr)];
    tag = get_tag(addr);

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
        } else if (target->is_valid) {
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
