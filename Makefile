CC=gcc
CFLAGS=-Wall -g

#TARGET_LRU=lru
#TARGET_LRU=pseudo_lru

ifeq ($(TARGET_LRU), lru)
 OBJS=main.o cache.o lru.o
else
 OBJS=main.o cache.o pseudo_lru.o
endif

TARGET=simulator

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
	rm *.o
	rm $(TARGET)
