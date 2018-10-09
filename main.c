#include "cache.h"

extern struct basic_cache cache;

FILE *fp;
char input_char;
uint64_t input_addr;

int main(int argc, char *argv[]) {
    cache.create();

    // Something goes on
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "File open ERROR\n");
    }

    while (fscanf(fp, "%c %lx\n", &input_char, &input_addr) != EOF) {
        if (input_char == 'R') {
            cache.read(input_addr);

        } else if (input_char == 'W') {
            cache.write(input_addr);
        }
    }

    cache.destroy();
    return 0;
}
