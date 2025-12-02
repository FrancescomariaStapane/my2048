#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "game.h"
#include "input.h"
#include "renderer.h"
#include<signal.h>
#include<stdlib.h>
#include<termios.h>
#include<unistd.h>


static int exit_loop = 0;
void signal_handler(){
    exit_loop = 1;
}

void test(){
	// Component* component = malloc(10*sizeof(Component));
	// load_digits(component);
	// for (int i = 0; i< 10; i++) {
 //        sleep(1);
	// 	printComponent(component[i]);
	// 	// printf("\n");
	// }
 //
	// while (1){}
}

void updateGameCell(BoardComponent* bc, int i, int j, int value, int fontCode) {
	int x = 0;
	int y = 0;
	char* workingDir = "."; // todo da cambiare
	char* fontDir = "/resources/pipeSeriff/";
	char* fileName = malloc(sizeof(char) * (strlen(fontDir) + strlen(workingDir) + 10));
	sprintf(fileName, "%s%s%d.txt",workingDir,fontDir,value);
	Component tmpCmp;
	newComponent(&tmpCmp, bc->cell_height, bc->cell_width);

	if (readCellFromFile(fileName, &tmpCmp,bc->cell_height, bc->cell_width) < 0) {
		exit_loop = 1;
	}
	// for (int i_ = 0; i_< tmpCmp.height; i_++) {
	// 	for (int j_ = 0; j_ < tmpCmp.width; j_++) {
	// 		tmpCmp.pixels[i_][j_].styleCode = value < 12 ? value : getStyleCode(OVER_4096);
	// 	}
	// }
	styleAllInComponent(&tmpCmp, value < 12 ? value : getStyleCode(OVER_4096));
	getPosOfBoardComponentCell(*bc,i,j, &x, &y);
	copySubComponentInComponent(tmpCmp, &bc->component,x,y);
	free(fileName);
	freeComponent(&tmpCmp);
}


void updateScoreBoard(BoardComponent* sc, board_state curState, board_state prevState, Component* digits, Component scoreText, int* numberDecomposition) {
	int topOffsetY = 2;
	copySubComponentInComponent(scoreText, &sc->component, getXOffsetToCenterComponent(sc->component.width, scoreText.width), topOffsetY);

	if (curState.score != prevState.score) {
		int n_digits;
		decomposeNumber(curState.score, &n_digits, numberDecomposition);
		int scoreOffsetY = topOffsetY + scoreText.height + 1;
		int scoreOffsetX = getXOffsetToCenterComponent(sc->component.width,3*n_digits);
		for (int i = n_digits -1; i >=0; i--) {
			copySubComponentInComponent(digits[numberDecomposition[i]],&sc->component, scoreOffsetX + 3*(n_digits-i-1), scoreOffsetY);
		}
	}
}

void updateInfoBoard(BoardComponent* ic, Component infoText) {
	loadInfo(&infoText);
	copySubComponentInComponent(infoText, &ic->component, 1,1);
}

void setupScreen(Screen * screen, Screen* nextScreen, BoardComponent* gameBoard, BoardComponent* scoreBoard, BoardComponent* infoBoard, int n_rows, int n_cols) {
	int horizontalDistance = 3;
	int verticalDistance = 2;
	newBoardComponent(gameBoard,n_rows, n_cols, 7, 15);
	nextScreen->panels[0].component = &gameBoard->component;
	nextScreen->panels[0].offset_x = horizontalDistance;
	nextScreen->panels[0].offset_y = verticalDistance;
	nextScreen->panels[1].offset_x = getXOffsetRightOfPanel(nextScreen->panels[0]) + horizontalDistance ;
	nextScreen->panels[1].offset_y = verticalDistance ;
	nextScreen->panels[2].offset_x = horizontalDistance ;
	nextScreen->panels[2].offset_y = getYOffsetDownPanel(nextScreen->panels[0]) + verticalDistance ;

	newBoardComponent(scoreBoard,1,1,gameBoard->component.height - 2, 36);
	nextScreen->panels[1].component = &scoreBoard->component;

	newBoardComponent(infoBoard,1,1, 7,  nextScreen->panels[1].component->width + nextScreen->panels[1].offset_x - 5);
	nextScreen->panels[2].component = &infoBoard->component;

	drawGameGrid(gameBoard);
	drawGameGrid(scoreBoard);
	drawGameGrid(infoBoard);
	copyScreen(screen, nextScreen);
	clearScreen(screen);
}




int n_rows, n_cols;
int seed;
board_state cur_state;
board_state prev_state;
Component digitsComponents[10];
int numberDecomposition[16];
Component scoreText;
Component infoText;
Component scoreNumber;
Screen screen;
Screen nextScreen;
struct  timespec req ={};
struct  timespec rem ={};
int game_over = 0;
char input;
int undos = 2;
BoardComponent gameBoard;
BoardComponent scoreBoard;
BoardComponent infoBoard;

int main(int argc, char** argv){
    configure_terminal();
    signal(SIGINT, signal_handler);

	if(argc == 3){
		n_cols = atoi(argv[1]);
		n_rows =atoi(argv[2]);
	}else{
        n_cols = 4;
        n_rows = 4;
    }
	seed=time(NULL);
	srand(seed);


	if(new_board_state(&cur_state, n_rows, n_cols) || new_board_state(&prev_state, n_rows, n_cols)){
		printf("could not instantiate game\n");
		return -1;
	}

	//digits are used to display score in score component
	for (int i=0; i < 10; i++)
		newComponent(&digitsComponents[i],3,3);
    newComponent(&scoreText,3,16);
    newComponent(&infoText,7,85);

	load_digits(&scoreText, digitsComponents);
	newScreen(&screen, 3);
	newScreen(&nextScreen, 3);

	// test();
	// while(!exit_loop){sleep(1);}

	while(!exit_loop){
        // printf("NEW GAME\n\n");
		init_game(&cur_state);
		setupScreen(&screen, &nextScreen, &gameBoard, &scoreBoard, &infoBoard, n_rows, n_cols);
		for (int i = 0; i < n_rows; i++) {
			for (int j = 0; j < n_cols; j++) {
				updateGameCell(&gameBoard, i, j, cur_state.values[i][j],0);
			}
		}
		updateScoreBoard(&scoreBoard, cur_state, prev_state, digitsComponents, scoreText, numberDecomposition);
		updateInfoBoard(&infoBoard, infoText);
		render(screen, nextScreen);

		// print_state(cur_state);
		do{
			input = read_input();
            if(input=='s'){
                break;
            }
			if(input!='0'){
                game_over = step(&cur_state, input, &prev_state, &undos);

				for (int i = 0; i < n_rows; i++) {
					for (int j = 0; j < n_cols; j++) {
						updateGameCell(&gameBoard, i, j, cur_state.values[i][j],0);
					}
				}
				updateScoreBoard(&scoreBoard, cur_state, prev_state, digitsComponents, scoreText, numberDecomposition);
				updateInfoBoard(&infoBoard, infoText);
				render(screen, nextScreen);
				copyScreen(&screen, &nextScreen);

                // print_state(cur_state);
                // fprintf(stderr,"\n");
                req.tv_nsec = 0.1 * 100000000; // 0.01 seconds
                nanosleep(&req, &rem);
            }

		}while(!game_over && !exit_loop);

	}
	freeScreen(&screen);
	freeScreen(&nextScreen);
	free_board_state(&cur_state);
    free_board_state(&prev_state);
}

