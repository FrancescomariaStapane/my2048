#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N_ROWS 4
#define N_COLS 4
typedef struct Board_State{

int values[N_COLS][N_ROWS];
}board_state;

void init_game(board_state* state);
void print_state(board_state state);
void get_rand_square(int* position);
void get_rand_empty_square(board_state state, int* position);
int get_new_tile();
void crunch_board(board_state* state, char move);
void crunch_line(int* line, int len);
int get_new_tile_value();
int step(board_state* state, char move);
int fill_new_square(board_state* state);


int main(int argc, char** argv){
	int seed=time(NULL);
	srand(seed);
	board_state state;
	init_game(&state);
	print_state(state);
	
	while(1){
		int game_over = 0;
		do{
			char move;
			printf("input next move:\n");
			scanf(" %c",&move);
			game_over = step(&state, move);
			print_state(state);
		}while(!game_over);
	}
	printf("GAME OVER\n");
	init_game(&state);
	/*printf("\n");
	crunch_board(&state,'r');
	printf("finale state:\n");
	print_state(state);
	printf("\n");
	init_game(&state);
	crunch_board(&state,'l');
	//print_state(state);
	printf("\n");
	printf("final state:\n");
	print_state(state);
	printf("\n");
	init_game(&state);
	crunch_board(&state,'u');
	//print_state(state);
	printf("final state:\n");
	print_state(state);

	printf("\n");
	init_game(&state);
	crunch_board(&state,'d');
	printf("finale state:\n");
	print_state(state);

	//print_state(state);
	printf("\n");
	*/
	/*
	int *line = malloc(8 *sizeof(int));
	int l[8]={2,4,8,16,32,64,128,256};
	//int l[8]={2,2,4,0,8,8,8,2};

	line = l;
	for (int i = 0; i< 8; i++){
		printf("%d ",line[i]);
	}
	
	//crunch_line(line, 8);
	crunch_line(line, 8);

	for (int i = 0; i< 8; i++){
		printf("%d ",line[i]);
	}
	printf("\n");
	*/
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

int step(board_state* state, char move){
	crunch_board(state, move);
	return fill_new_square(state);
}
