CC=gcc
CFLAGS=-Wall -g
OBJS=main.o cache.o lru.o
TARGET=simulator

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

clean:
	rm *.o
	rm $(TARGET)
