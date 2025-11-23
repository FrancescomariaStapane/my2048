#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

//#define N_ROWS 4
//#define N_COLS 4
typedef struct Board_State{
	int* v;
	//int values[N_COLS][N_ROWS];
	int ** values;
}board_state;

void init_game(board_state* state);
void print_state(board_state state);
void get_rand_square(int* position);
void get_rand_empty_square(board_state state, int* position);
int get_new_tile();
void crunch_board(board_state* state, char move);
void crunch_line(int* line, int len);
int get_new_tile_value();
int step(board_state* state, char move, board_state* prev_state, int *undos);
int fill_new_square(board_state* state);
int new_board_state(board_state* state);
void free_board_state(board_state* state);
void copy_board_state(board_state* src, board_state* dst);
bool are_boards_equal(board_state s1, board_state s2);
bool check_game_over(board_state state);

int N_COLS = 4;
int N_ROWS = 4;
int undos = 2;

int main(int argc, char** argv){
	if(argc == 3){
		N_COLS = atoi(argv[1]);
		N_ROWS =atoi(argv[2]);

	}
	int seed=time(NULL);
	srand(seed);
	board_state cur_state;
	board_state prev_state;

	if(new_board_state(&cur_state) || new_board_state(&prev_state)){
		printf("could not instantiate game\n");
		return -1;
	}

	printf("moves:\nl -> left\nr -> right\nu -> up\nd -> down\nc -> undo\n\n");
	init_game(&cur_state);
	print_state(cur_state);
	while(1){
		int game_over = 0;
		do{
			char move;
			printf("input next move:\n");
			scanf(" %c",&move);
			game_over = step(&cur_state, move, &prev_state, &undos);
			print_state(cur_state);
			printf("\n");
		}while(!game_over);
		printf("GAME OVER\n");
		init_game(&cur_state);
		print_state(cur_state);
	}
	free_board_state(&cur_state);
}


int new_board_state(board_state* state){
	if((state->v = malloc(N_ROWS * N_COLS * sizeof(int))) == NULL)
		return -1;
	if ((state->values = malloc(N_ROWS * sizeof(int*))) == NULL)
		return -1;
	for(int i=0; i< N_ROWS;i++){
		state->values[i]=&(state->v[N_COLS * i]);
	}
	return 0;
}
void free_board_state(board_state* state){
	//for(int i = 0; i < N_ROWS; i++){
	//	free(state->values[i]);
	//}
	free(state->values);
	free(state->v);

}
void init_game(board_state* state){
	for(int i=0; i<N_ROWS; i++) {
		for(int j=0; j<N_COLS; j++){
			state->values[i][j]=0;
			//state->values[i][j]= i*N_COLS + j;
		}
	}
	fill_new_square(state);
	fill_new_square(state);
}


void print_state(board_state state){
	for(int i=0; i<N_ROWS; i++){
		for(int j=0; j<N_COLS; j++){
			printf("%d ",state.values[i][j]);
		}
		printf("\n");
	}

}

void get_rand_square(int* position){
	int row = rand()%N_ROWS;
	int col = rand()%N_COLS;
	position[0]=row;
	position[1]=col;
}

void get_rand_empty_square(board_state state, int* position){
	//https://stackoverflow.com/questions/966108/choose-random-array-element-satisfying-certain-property
	position[0]=-1;	
	position[1]=-1;	
	int  count = 0;
	for(int i =0; i<N_ROWS; i++){
		for(int j=0; j<N_COLS; j++){
			if(!state.values[i][j] && !(rand()% ++count) ){
 				position[0] = i;
				position[1] = j;
				
			}
		}
	}
}

int get_new_tile(){
	return 2;
}

void crunch_board(board_state* state, char move){
	int k=0;
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
		int* row_copy=(int *)malloc(N_COLS *sizeof(int));							 
		for (int i=0; i< N_ROWS; i++){
			int start = orientation < 0 ? N_COLS -1 : 0;
			int end = orientation < 0 ? 0 : N_COLS -1;
			for(int j = start, k=0; k<N_COLS; j+=orientation, k++){
				row_copy[k]=state->values[i][j];
			}
			crunch_line(row_copy, N_COLS);
			for(int j = start, k=0; k<N_COLS; j+=orientation, k++){
				state->values[i][j]=row_copy[k];
			}
			
		}
		free(row_copy);
	}
	else{ //vertical
		int* column_copy=malloc(N_ROWS*sizeof(int));							 
		for (int j=0; j< N_COLS; j++){
			int start = orientation < 0 ? N_ROWS -1 : 0;
			int end = orientation < 0 ? 0 : N_ROWS -1;
			for(int i = start, k=0; k<N_ROWS; i+=orientation, k++){
				column_copy[k]=state->values[i][j];
			
			}
			crunch_line(column_copy,N_ROWS);
			for(int i = start, k=0; k<N_ROWS; i+=orientation, k++){
				state->values[i][j]=column_copy[k];
					
			}

		}
		free(column_copy);
	}
	return;
}
int get_new_tile_value(){
	//return 2 with p = 0.9 and 4 with p = 0.1
	return rand()%10 == 0 ? 4 : 2;
}
void crunch_line(int* line, int len){
	//il wall rappresenta la tile su cui va a sbattere la tile successiva non nulla durante il crunch
	//generalmente wall è non 0, ma lo è se è avvenuto un raddoppio, nella cella che è sparita
	int wall = 0;
	for (int i=1; i<len;i++){
		// printf("%d ",*(*line+i));
		if(line[i]==0){
			continue;
		}
		if(line[i]==line[wall]){
			line[wall]<<=1;
			line[i]=0;
			wall++;
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
	printf("\n");
}

int fill_new_square(board_state* state){
	int new_tile_pos[2];
	get_rand_empty_square(*state, new_tile_pos);
	if(new_tile_pos[0] < 0){
		return -1;
	}
	state->values[new_tile_pos[0]][new_tile_pos[1]] = get_new_tile_value();
	return 0;
}

int step(board_state* state, char move, board_state* prev_state, int *undos){
	bool is_new_move=true;
	board_state tmp_state;
	new_board_state(&tmp_state);
	copy_board_state(state, &tmp_state);
	if(move == 'c'){
		is_new_move = false;
		copy_board_state(prev_state, state);
	}
	else{
		crunch_board(state, move);
		//printf("prevstate: {\n");
		//print_state(*prev_state);
		//printf("\n");
		//for(int i = 0; i< N_COLS * N_ROWS; i++){
		//	printf("%d ", prev_state->v[i]);
		//}
		//printf("}\n");
		is_new_move = !are_boards_equal(tmp_state, *state);

	}
	if(is_new_move){
		
		fill_new_square(state);
		copy_board_state(&tmp_state, prev_state);
	}
	free_board_state(&tmp_state);
	return check_game_over(*state); // return check_game_over(state)
}
bool are_boards_equal(board_state s1, board_state s2){
	for(int i = 0; i< N_ROWS * N_COLS; i++){
		if(s1.v[i]!=s2.v[i]){
			return false;
		}
	}
	return true;
}
void copy_board_state(board_state *src, board_state* dst){
	memcpy(dst->v, src->v, N_COLS*N_ROWS * sizeof(int));
}

bool check_game_over(board_state state){
	for(int i = 0; i < N_ROWS; i++){
		for( int j = 0; j < N_COLS; j++){
			if(state.values[i][j] == 0){
				return false;
			}
			if(j < N_COLS -1 && state.values[i][j] == state.values[i][j+1]){
				return false;
			}
			if(i < N_ROWS -1 && state.values[i][j] == state.values[i+1][j]){
				return false;
			}

		}
	}
	return true;
}
