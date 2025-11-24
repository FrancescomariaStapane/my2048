ROOT = .
LIB2048GAME = lib2048game.a
OBJS = 2048game.o input.o

all: 2048 

$(LIB2048GAME): $(OBJS)
	ar -rcs $@ $?

%: %.c $(LIB2048GAME)
	gcc -ggdb -o $@ $@.c  -L$(ROOT) -l2048game
