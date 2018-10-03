#include "cache.h"

extern struct cache_info cache;

int main() {
    cache.create();

    // Something goes on

    cache.destroy();
    return 0;
}
