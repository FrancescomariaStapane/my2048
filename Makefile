
CC := gcc
AR := ar
ROOT := .
LIB2048 := lib2048.a
OBJS := game.o input.o renderer.o storage.o
CFLAGS := -ggdb -O0 -fno-inline -fno-omit-frame-pointer -Wall -Wextra
LDFLAGS := -L$(ROOT) -l2048 -lsqlite3

.PHONY: all clean

all: 2048

$(LIB2048): $(OBJS)
	$(AR) -rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

2048: 2048.c $(LIB2048)
	$(CC) $(CFLAGS) -o $@ 2048.c $(LDFLAGS)

clean:
	rm -f 2048 $(LIB2048) *.o
