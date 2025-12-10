#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "game.h"
//#define state->n_rows 4
//#define state->n_cols 4







int newBoardState(BoardState* state, int n_rows, int n_cols){
	if((state->v = malloc(n_cols * n_rows * sizeof(int))) == NULL)
		return -1;
	if ((state->values = malloc(n_rows * sizeof(int*))) == NULL)
		return -1;
	for(int i=0; i< n_rows;i++){
		state->values[i]=&(state->v[n_cols * i]);
	}
	state->n_rows = n_rows;
	state->n_cols = n_cols;
	return 0;
}
void freeBoardState(BoardState* state){
	//for(int i = 0; i < state->n_rows; i++){
	//	free(state->values[i]);
	//}
	free(state->values);
	free(state->v);

}
void initGame(BoardState* state){
	for(int i=0; i<state->n_rows; i++) {
		for(int j=0; j<state->n_cols; j++){
			state->values[i][j]=0;
			//state->values[i][j]= i*state->n_cols + j;
		}
	}
	fill_new_square(state);
	fill_new_square(state);

	// state->maxTile=2; // cancella prev_state->v ???
	state->score=0;
}

void getWorkingDir(char* cwd) {
	strcpy(cwd, "/home/franc/Desktop/2048");
}

void printState(BoardState state){
	FILE* out_file = stderr;
	for(int i=0; i<state.n_rows; i++){
		for(int j=0; j<state.n_cols; j++){
			int value = state.values[i][j];
			fprintf(out_file,"%d ", value ? 1 << value : 0);
		}
		fprintf(out_file,"\n");
	}
	fprintf(out_file,"score: %d\nmax tile: %d",state.score,state.maxTile);
}

/*void get_rand_square(int* position, int n_rows, int n_cols){
	int row = rand()%n_rows;
	int col = rand()%n_cols;
	position[0]=row;
	position[1]=col;
}*/

void getRandEmptySquare(BoardState state, int* position){
	//https://stackoverflow.com/questions/966108/choose-random-array-element-satisfying-certain-property
	position[0]=-1;	
	position[1]=-1;	
	int  count = 0;
	for(int i =0; i<state.n_rows; i++){
		for(int j=0; j<state.n_cols; j++){
			if(!state.values[i][j] && !(rand()% ++count) ){
 				position[0] = i;
				position[1] = j;
				
			}
		}
	}
}

void crunch_board(BoardState* state, char move){
	// int k=0;
	//\. differntiate only horizontal or vertizal
	//2. if horizontal iterate through rows, if vertical iterate through columns
	//3. copy vector to crunch, if orientation is -1 reverse it. Pass it to crunch function
	//4. crunch function crunches the vector
	//5. copy crunched vector to board state column or row, if orientation is -1, coopy it reversed
	if (move != 'r' && move != 'l' && move != 'u' && move != 'd'){
		return;
	}
	int orientation = 1;
	if(move == 'r' || move == 'd'){
		orientation = -1;
	}
	if (move == 'r' || move == 'l'){ //horizontal
		int* row_copy=malloc(state->n_cols *sizeof(int));
		for (int i=0; i< state->n_rows; i++){
			int start = orientation < 0 ? state->n_cols -1 : 0;
			// int end = orientation < 0 ? 0 : state->n_cols -1;
			for(int j = start, k=0; k<state->n_cols; j+=orientation, k++){
				row_copy[k]=state->values[i][j];
			}
			crunch_line(row_copy, state->n_cols, state);
			for(int j = start, k=0; k<state->n_cols; j+=orientation, k++){
				state->values[i][j]=row_copy[k];
			}
			
		}
		free(row_copy);
	}
	else{ //vertical
		int* column_copy=malloc(state->n_rows*sizeof(int));
		for (int j=0; j< state->n_cols; j++){
			int start = orientation < 0 ? state->n_rows -1 : 0;
			// int end = orientation < 0 ? 0 : state->n_rows -1;
			for(int i = start, k=0; k<state->n_rows; i+=orientation, k++){
				column_copy[k]=state->values[i][j];
			
			}
			crunch_line(column_copy,state->n_rows, state);
			for(int i = start, k=0; k<state->n_rows; i+=orientation, k++){
				state->values[i][j]=column_copy[k];
					
			}

		}
		free(column_copy);
	}
	return;
}
int get_new_tile_value(){
	//return 2 with p = 0.9 and 4 with p = 0.1
	return rand()%10 == 0 ? 2 : 1;
}
void crunch_line(int* line, int len, BoardState* state){
	//il wall rappresenta la tile su cui va a sbattere la tile successiva non nulla durante il crunch
	//generalmente wall è non 0, ma lo è se è avvenuto un raddoppio, nella cella che è sparita
	int wall = 0;
	for (int i=1; i<len;i++){
		// printf("%d ",*(*line+i));
		if(line[i]==0){
			continue;
		}
		if(line[i]==line[wall]){
			line[wall]+=1;
			line[i]=0;
			int new_tile = 1<<line[wall];
			wall++;
			state -> score += new_tile;
			if (line[wall-1] > state -> maxTile) {
				state -> maxTile = line[wall-1];
			}
			continue;
		}
		if(line[wall]!=0){
			wall++;
		}
		if( i != wall){
			line[wall]=line[i];
			line[i]=0;
			}	
		}
	// printf("\n");
}

int fill_new_square(BoardState* state){
	int new_tile_pos[2];
	getRandEmptySquare(*state, new_tile_pos);
	if(new_tile_pos[0] < 0){
		return -1;
	}
	state->values[new_tile_pos[0]][new_tile_pos[1]] = get_new_tile_value();
	return 0;
}

int step(BoardState* state, char move, BoardState* prev_state, int *undos){
	bool is_new_move=true;
	BoardState tmp_state;
	newBoardState(&tmp_state, state->n_rows, state->n_cols);
	copyBoardState(state, &tmp_state);
	if(move == 'c'){
		is_new_move = false;
		copyBoardState(prev_state, state);
	}
	else{
		crunch_board(state, move);
		//printf("prevstate: {\n");
		//print_state(*prev_state);
		//printf("\n");
		//for(int i = 0; i< state->n_cols * state->n_rows; i++){
		//	printf("%d ", prev_state->v[i]);
		//}
		//printf("}\n");
		is_new_move = !are_boards_equal(tmp_state, *state);

	}
	if(is_new_move){
		
		fill_new_square(state);
		copyBoardState(&tmp_state, prev_state);
	}
	freeBoardState(&tmp_state);
	return check_game_over(*state); // return check_game_over(state)
}
bool are_boards_equal(BoardState s1, BoardState s2){
	for(int i = 0; i< s1.n_rows * s1.n_cols; i++){
		if(s1.v[i]!=s2.v[i]){
			return false;
		}
	}
	return true;
}
void copyBoardState(BoardState *src, BoardState* dst){
	memcpy(dst->v, src->v, src->n_cols*src->n_rows * sizeof(int));
	dst->maxTile = src->maxTile;
	dst->score = src->score;
	dst->n_cols = src->n_cols;
	dst->n_rows = src->n_rows;
}

bool check_game_over(BoardState state){
	for(int i = 0; i < state.n_rows; i++){
		for( int j = 0; j < state.n_cols; j++){
			if(state.values[i][j] == 0){
				return false;
			}
			if(j < state.n_cols -1 && state.values[i][j] == state.values[i][j+1]){
				return false;
			}
			if(i < state.n_rows -1 && state.values[i][j] == state.values[i+1][j]){
				return false;
			}

		}
	}
	return true;
}

void boardStateToStr(BoardState state, char* buf) {
	int offset = sprintf(buf,"%d;%d;%d;%d;",state.n_cols,state.n_rows,state.score,state.maxTile);
	for(int i = 0; i < state.n_rows; i++) {
		for( int j = 0; j < state.n_cols; j++) {
			offset += sprintf(buf + offset, "%d ", state.values[i][j]);
		}
	}
	buf[offset-1] = 0;
}

int strToBoardState(BoardState* state, char* str) {
	int n_cols, n_rows;
	if (!state || !state->v || !state->values)
		return -1;
	char copy[4096];
	strcpy(copy, str);
	char* token = strtok(copy, ";");
	n_cols = atoi(token);
	token = strtok(NULL, ";");
	n_rows = atoi(token);
	token = strtok(NULL, ";");
	state->score = atoi(token);
	token = strtok(NULL, ";");
	state->maxTile = atoi(token);
	if (n_cols != state->n_cols || n_rows != state->n_rows)
		return  -1;
	for(int i = 0; i < state->n_rows; i++) {
		for( int j = 0; j < state->n_cols; j++) {
			token = strtok(NULL, " ");
			if (token == NULL)
				return  -1;
			state->values[i][j]=atoi(token);
		}
	}
	return 0;

}