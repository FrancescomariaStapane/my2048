#ifndef LIBGAME_GAME_H
#define LIBGAME_GAME_H
#include<stdbool.h>

typedef struct Board_State{
	int* v;
	//int values[state->n_cols][state->n_rows];
	int ** values;
	int n_rows, n_cols;
	int score;
	int maxTile;
}BoardState;

// int undos = 2;
void init_game(BoardState* state);
void print_state(BoardState state);
//void get_rand_square(int* position);
void get_rand_empty_square(BoardState state, int* position);
void crunch_board(BoardState* state, char move);
void crunch_line(int* line, int len, BoardState* state);
int get_new_tile_value();
int step(BoardState* state, char move, BoardState* prev_state, int *undos);
int fill_new_square(BoardState* state);
int new_board_state(BoardState* state, int n_rows, int n_cols);
void free_board_state(BoardState* state);
void copy_board_state(BoardState* src, BoardState* dst);
bool are_boards_equal(BoardState s1, BoardState s2);
bool check_game_over(BoardState state);

#endif