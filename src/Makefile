CFLAGS=-std=c99 -lm -march=native -O2
DFLAGS=-g3 -ggdb3
SOURCES=libbitmap.c main.c


all: ALL

ALL: libbitmap

libbitmap: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o main


clean:
	rm -f main *.o invert4.bmp
