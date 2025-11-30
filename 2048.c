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
	BoardComponent bc1;
    BoardComponent bc2;
    BoardComponent bc3;
    // Component cp;
    
    newBoardComponent(&bc1,1,1,7,15);
    newBoardComponent(&bc2,1,1,7,15);
    newBoardComponent(&bc3,4,4,7,15);
    
    Screen screen;
    Screen neextScreen;
    newScreen(&screen, 3);
    newScreen(&neextScreen, 1);
    screen.panels[0].component = &bc1.component;
    screen.panels[1].component = &bc2.component;
    screen.panels[2].component = &bc3.component;

    screen.panels[0].offset_x = 3;
    screen.panels[0].offset_y = 3;

    //todo funzione  int offset_after horizontally/ vertically (Panel panel, additional offset x, y)
    screen.panels[1].offset_x = getXOffsetRightOfPanel(screen.panels[0]) +2;
    screen.panels[1].offset_y = screen.panels[0].offset_y;
    screen.panels[2].offset_x = screen.panels[0].offset_x;
    screen.panels[2].offset_y = getYOffsetDownPanel(screen.panels[0]) +3;


    copyScreen(&neextScreen, &screen);
    clearScreen(&screen);
	// printScreen((screen));
    render(screen, neextScreen);
    copyScreen(&screen, &neextScreen);
    int x = 0;
	int y = 0;
    // strcpy(board->component.pixels[i][j].value,"█");
	Component tmpCmp;
	newComponent(&tmpCmp, bc3.cell_height, bc3.cell_width);
	readCellFromFile("resources/pipeSeriff/5.txt", &tmpCmp,bc3.cell_height, bc3.cell_width);
	for (int i = 0; i< tmpCmp.height; i++) {
		for (int j = 0; j < tmpCmp.width; j++) {
			tmpCmp.pixels[i][j].styleCode = (i+j) % 2 ? 10 : 11;
		}
	}
	for (int i = 0; i< bc3.n_rows; i++) {
		for (int j = 0; j < bc3.n_cols; j++) {
			int x = 0;
			int y = 0;
			getPosOfBoardComponentCell(bc3,i,j, &x, &y);
			copySubComponentInComponent(tmpCmp, neextScreen.panels[2].component,x,y);

		}
	}
	freeComponent(&tmpCmp);

    // printScreen(neextScreen);

    sleep(1);
    render(screen,neextScreen);

}

void updateGameCell(BoardComponent* bc, int i, int j, int value, int fontCode) {
	char* folderName = "resources/pipeSeriff";
	int x = 0;
	int y = 0;
	char* workingDir = "/home/franc/Desktop/2048"; // todo da cambiare
	char* fontDir = "/resources/pipeSeriff/";
	char* fileName = malloc(sizeof(char) * (strlen(fontDir) + strlen(workingDir) + 10));
	sprintf(fileName, "%s%s%d.txt",workingDir,fontDir,value);
	Component tmpCmp;
	newComponent(&tmpCmp, bc->cell_height, bc->cell_width);

	if (readCellFromFile(fileName, &tmpCmp,bc->cell_height, bc->cell_width) < 0) {
		exit_loop = 1;
	}
	for (int i = 0; i< tmpCmp.height; i++) {
		for (int j = 0; j < tmpCmp.width; j++) {
			tmpCmp.pixels[i][j].styleCode = value < 12 ? value : getStyleCode(OVER_4096);
		}
	}

	getPosOfBoardComponentCell(*bc,i,j, &x, &y);
	copySubComponentInComponent(tmpCmp, &bc->component,x,y);

	freeComponent(&tmpCmp);
}


void setUpScreen(Screen * screen, Screen* nextScreen, BoardComponent* gameBoard, BoardComponent* scoreBoard, BoardComponent* infoBoard, int n_rows, int n_cols) {
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

	newBoardComponent(scoreBoard,1,1,gameBoard->component.height - 2, 30);
	nextScreen->panels[1].component = &scoreBoard->component;

	newBoardComponent(infoBoard,1,1, 7,  nextScreen->panels[1].component->width + nextScreen->panels[1].offset_x - 5);
	nextScreen->panels[2].component = &infoBoard->component;

	drawGameGrid(gameBoard);
	drawGameGrid(scoreBoard);
	drawGameGrid(infoBoard);

	copyScreen(screen, nextScreen);
	clearScreen(screen);
}





int main(int argc, char** argv){
    configure_terminal();


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


	Screen screen;
	Screen nextScreen;
	newScreen(&screen, 3);
	newScreen(&nextScreen, 3);

	BoardComponent gameBoard;
	BoardComponent scoreBoard;
	BoardComponent infoBoard;

    int undos = 2;

    signal(SIGINT, signal_handler);

    struct  timespec req ={};
    struct  timespec rem ={};

	// test();
	// while(!exit_loop){sleep(1);}

	while(!exit_loop){
		int game_over = 0;
        // printf("NEW GAME\n\n");
		init_game(&cur_state);
		setUpScreen(&screen, &nextScreen, &gameBoard, &scoreBoard, &infoBoard, n_rows, n_cols);
		for (int i = 0; i < n_rows; i++) {
			for (int j = 0; j < n_cols; j++) {
				updateGameCell(&gameBoard, i, j, cur_state.values[i][j],0);
			}
		}
		render(screen, nextScreen);

		// print_state(cur_state);
		do{
			char input = read_input();
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
				render(screen, nextScreen);
				copyScreen(&screen, &nextScreen);
                // print_state(cur_state);
                // printf("\n")
                req.tv_nsec = 0.1 * 100000000; // 0.01 seconds
                nanosleep(&req, &rem);

            }

		}while(!game_over && !exit_loop);

	}
	free_board_state(&cur_state);
    free_board_state(&prev_state);
}

