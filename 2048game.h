#ifndef LIB2048GAME_2048GAME_H
#define LIB2048GAME_2048GAME_H

typedef struct Board_State{
	int* v;
	//int values[state->n_cols][state->n_rows];
	int ** values;
	int n_rows, n_cols;
}board_state;

// int undos = 2;
void init_game(board_state* state);
void print_state(board_state state);
//void get_rand_square(int* position);
void get_rand_empty_square(board_state state, int* position);
int get_new_tile();
void crunch_board(board_state* state, char move);
void crunch_line(int* line, int len);
int get_new_tile_value();
int step(board_state* state, char move, board_state* prev_state, int *undos);
int fill_new_square(board_state* state);
int new_board_state(board_state* state, int n_rows, int n_cols);
void free_board_state(board_state* state);
void copy_board_state(board_state* src, board_state* dst);
bool are_boards_equal(board_state s1, board_state s2);
bool check_game_over(board_state state);

#endif