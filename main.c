#include "cache.h"

extern struct cache_info cache;
int capacity, n_way, block_sz;

FILE *fp;
char input_char;
uint64_t input_addr;

int main(int argc, char *argv[]) {
    fp = fopen("config", "r");
    if (fp == NULL) {
        fprintf(stderr, "No config file\n");
        exit(1);
    }
    fscanf(fp, "%d %d %d", &capacity, &n_way, &block_sz);

    cache.create(&cache, capacity, n_way, block_sz);

    // Something goes on
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "File open ERROR\n");
        exit(1);
    }

    while (fscanf(fp, "%c %lx\n", &input_char, &input_addr) != EOF) {
        if (input_char == 'R') {
            cache.read(input_addr);

        } else if (input_char == 'W') {
            cache.write(input_addr);
        }
    }

    cache.destroy(&cache);
    return 0;
}
