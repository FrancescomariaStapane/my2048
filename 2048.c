#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "2048game.h"
#include "input.h"
#include<signal.h>
#include<stdlib.h>
#include<termios.h>

static int exit_loop = 0;
void signal_handler(){
    exit_loop = 1;
}
static struct termios old_termios, new_termios;

int main(int argc, char** argv){
	
	int n_rows, n_cols;
	if(argc == 3){
		n_cols = atoi(argv[1]);
		n_rows =atoi(argv[2]);			
	}else{
        n_cols = 4;
        n_rows = 4;
    }
	int seed=time(NULL);
	srand(seed);
	board_state cur_state;
	board_state prev_state;

	if(new_board_state(&cur_state, n_rows, n_cols) || new_board_state(&prev_state, n_rows, n_cols)){
		printf("could not instantiate game\n");
		return -1;
	}

	printf("moves:\nl -> left\nr -> right\nu -> up\nd -> down\nc -> undo\n\n");

    int undos = 2;

    configure_terminal();

    signal(SIGINT, signal_handler);

    struct  timespec req ={};
    struct  timespec rem ={};

	while(!exit_loop){
		int game_over = 0;
        printf("NEW GAME\n\n");
		init_game(&cur_state);
		print_state(cur_state);
		do{
			char move;
			char key = read_input();
            if(key=='s'){
                break;
            }

            // printf("%c\n",key);
            if(key!='0'){
                game_over = step(&cur_state, key, &prev_state, &undos);
                print_state(cur_state);
                printf("\n");
                req.tv_nsec = 0.1 * 100000000; // 0.01 seconds
                nanosleep(&req, &rem);
            }

		}while(!game_over && !exit_loop);

	}
	free_board_state(&cur_state);
    free_board_state(&prev_state);
}

