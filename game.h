#ifndef LIBGAME_GAME_H
#define LIBGAME_GAME_H
#include<stdbool.h>

typedef struct BoardState{
	int* v;
	//int values[state->n_cols][state->n_rows];
	int ** values;
	int n_rows, n_cols;
	int score;
	int maxTile;
}BoardState;

typedef struct UserState {
	char username[512];
	BoardState state;
	int bestScore;
	int bestTile;
}UserState;

typedef struct UserHighScore {
	char username[512];
	int bestScore;
	int bestTile;
}UserHighScore;

typedef struct Leaderboard {
	int nOfUsers;
	int iterator;
	UserHighScore* users;
}Leaderboard;
// int undos = 2;
void initGame(BoardState* state);
void printState(BoardState state);
//void get_rand_square(int* position);
void getRandEmptySquare(BoardState state, int* position);
void crunch_board(BoardState* state, char move);
void crunch_line(int* line, int len, BoardState* state);
int get_new_tile_value();
int step(BoardState* state, char move, BoardState* prev_state, int *undos);
int fill_new_square(BoardState* state);
int newBoardState(BoardState* state, int n_rows, int n_cols);
void freeBoardState(BoardState* state);
void copyBoardState(BoardState* src, BoardState* dst);
bool are_boards_equal(BoardState s1, BoardState s2);
bool check_game_over(BoardState state);
void boardStateToStr(BoardState state, char* buf);
int strToBoardState(BoardState* state, char* str);
void getWorkingDir(char* cwd);
#endif