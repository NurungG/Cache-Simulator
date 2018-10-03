#ifndef __CONTAINER_H__
#define __CONTAINER_H__

struct cache_info {
    void (*create) ();
    void (*destroy) ();
    void (*read) ();
    void (*write) ();

    struct cache_way *way;
    struct cache_set *set;
};

#endif
