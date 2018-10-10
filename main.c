#include <stdio.h>
#include <stdint.h>

#include "cache.h"

extern struct cache_info cache;


static void read_config(FILE *fp, int *_cap, int *_way, int *_blk) {
    fp = fopen("config", "r");
    if (fp == NULL) {
        fprintf(stderr, "No config file\n");
        exit(1);
    }
    fscanf(fp, "%d %d %d", _cap, _way, _blk);
    fclose(fp);
}

static void proceed_trace(FILE *fp, char *filename) {
    char op; uint64_t addr;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "File open ERROR\n");
        exit(1);
    }

    while (fscanf(fp, "%c %lx\n", &op, &addr) != EOF) {
        if (op == 'R') {
            cache.read(addr);
        } else if (op == 'W') {
            cache.write(addr);
        }
    }

    fclose(fp);
}

int main(int argc, char *argv[]) {
    FILE *fp = 0;
    int capacity, n_way, block_sz;

    read_config(fp, &capacity, &n_way, &block_sz);

    cache.create(&cache, capacity, n_way, block_sz);
    proceed_trace(fp, argv[1]);
    cache.destroy(&cache);

    return 0;
}
