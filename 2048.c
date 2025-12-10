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
#include "storage.h"
#include<unistd.h>
#include <sys/ioctl.h>
#include <pwd.h>

static int exit_loop = 0;
void signal_handler(){
    exit_loop = 1;
}


void test() {
}

bool isStateValid(BoardState state) {
	for (int i= 0; i < state.n_rows; i++)
		for (int j = 0; j < state.n_cols; j++)
			if (state.values[i][j]< 0 || state.values[i][j] > 24)
				return false;
	return true;
}



void updateGameCell(BoardComponent* bc, int i, int j, int value, int fontCode) {
	int x = 0;
	int y = 0;
	char workingDir[4096];
	getWorkingDir(workingDir);
	char* fontDir = "/resources/pipeSeriff/";
	char* fileName = malloc(sizeof(char) * (strlen(fontDir) + strlen(workingDir) + 10));
	sprintf(fileName, "%s%s%d.txt",workingDir,fontDir,value);
	Component tmpCmp;
	newComponent(&tmpCmp, bc->cell_height, bc->cell_width);

	if (readCellFromFile(fileName, &tmpCmp,bc->cell_height, bc->cell_width) < 0) {
		exit_loop = 1;
	}
	styleAllInComponent(&tmpCmp, value < 12 ? value : getStyleCode(OVER_4096));
	getPosOfBoardComponentCell(*bc,i,j, &x, &y);
	copySubComponentInComponent(tmpCmp, &bc->component,x,y);
	free(fileName);
	freeComponent(&tmpCmp);
}



void updateInfoBoard(BoardComponent* ic, Component infoText) {
	loadInfo(&infoText);
	copySubComponentInComponent(infoText, &ic->component, 2,1);
}



void setupScreen(Screen * screen, Screen* nextScreen, BoardComponent* gameBoard, BoardComponent* scoreBoard, BoardComponent* infoBoard, int n_rows, int n_cols) {
	int horizontalDistance = 3;
	int verticalDistance = 2;
	freeBoardComponent(gameBoard);
	newBoardComponent(gameBoard,n_rows, n_cols, 7, 15);
	nextScreen->panels[0].component = &gameBoard->component;
	nextScreen->panels[0].offset_x = horizontalDistance;
	nextScreen->panels[0].offset_y = verticalDistance;
	nextScreen->panels[1].offset_x = getXOffsetRightOfPanel(nextScreen->panels[0]) + horizontalDistance ;
	nextScreen->panels[1].offset_y = verticalDistance ;
	nextScreen->panels[2].offset_x = horizontalDistance ;
	nextScreen->panels[2].offset_y = getYOffsetDownPanel(nextScreen->panels[0]) + verticalDistance ;
	freeBoardComponent(scoreBoard);
	newBoardComponent(scoreBoard,1,1,gameBoard->component.height - 2, 36);
	nextScreen->panels[1].component = &scoreBoard->component;
	freeBoardComponent(infoBoard);
	newBoardComponent(infoBoard,1,1, 8,  nextScreen->panels[1].component->width + nextScreen->panels[1].offset_x - 5);
	nextScreen->panels[2].component = &infoBoard->component;

	drawGameGrid(gameBoard);
	drawGameGrid(scoreBoard);
	drawGameGrid(infoBoard);
	// copyScreen(screen, nextScreen);
	// clearScreen(screen);
}




int n_rows, n_cols;
int seed;
BoardState cur_state;
BoardState prev_state;
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
struct winsize sz;
char username[4096];
Leaderboard lb;
int rs;
bool ranked = 1;
UserState userState;
int bestScoreEver;
int bestTileEver;

void updateLeaderboard(BoardComponent* sc, Leaderboard lb) {
	int yOffsetHeading = 10;//to account for score display and padding
	int yOffsetUsers = yOffsetHeading + 2;
	int leaderBoardRows = sc->cell_height - yOffsetUsers -1;
	int maxUsernameChars = 16;
	int maxScoreChars = 7;
	int xOffsetPos = 2;
	int xOffsetUsername = xOffsetPos + 3;
	int xOffsetScore = xOffsetUsername + maxUsernameChars + 1;
	int xOffsetTile = xOffsetScore + maxScoreChars + 1;

	printlineInComponent("#", &scoreBoard.component, 0, xOffsetPos, yOffsetHeading, maxUsernameChars);
	printlineInComponent("Username", &scoreBoard.component, 0, xOffsetUsername, yOffsetHeading, maxUsernameChars);
	printlineInComponent("Score", &scoreBoard.component, 0, xOffsetScore, yOffsetHeading, maxUsernameChars);
	printlineInComponent("Tile", &scoreBoard.component, 0, xOffsetTile, yOffsetHeading, maxUsernameChars);

	bool foundSelf = false;
	int i;
	char buf[16];
	for (i = 0; i < lb.nOfUsers; i++) {
		if (i >= leaderBoardRows - 1) {
			break;
		}
		sprintf(buf,"%d", i+1);
		printlineInComponent(buf, &sc->component, 0, xOffsetPos, yOffsetUsers + i, maxUsernameChars);
		printlineInComponent(lb.users[i].username, &sc->component, 0, xOffsetUsername, yOffsetUsers + i, maxUsernameChars);
		sprintf(buf,"%d", lb.users[i].bestScore);
		printlineInComponent(buf, &sc->component, 0, xOffsetScore, yOffsetUsers + i, maxUsernameChars);
		sprintf(buf,"%d", 1<<lb.users[i].bestTile);
		printlineInComponent(buf, &sc->component, 0, xOffsetTile, yOffsetUsers + i, maxUsernameChars);
		if (!strcmp(lb.users[i].username, username)){
			foundSelf = true;
			for (int j = 0; j < sc->cell_width; j++) {
				sc->component.pixels[yOffsetUsers + i][j+1	].styleCode = lb.users[i].bestTile;
			}
		}
	}
	if (!foundSelf) {
		printlineInComponent("..", &sc->component, 0, xOffsetPos, yOffsetUsers + leaderBoardRows - 1, maxUsernameChars);
		printlineInComponent("..", &sc->component, 0, xOffsetUsername, yOffsetUsers + leaderBoardRows - 1, maxUsernameChars);
		printlineInComponent("..", &sc->component, 0, xOffsetScore, yOffsetUsers + leaderBoardRows - 1, maxUsernameChars);
		printlineInComponent("..", &sc->component, 0, xOffsetTile, yOffsetUsers + leaderBoardRows - 1, maxUsernameChars);
		while (strcmp(lb.users[i++].username, username)) {}

		sprintf(buf,"%d", i);
		printlineInComponent(buf, &sc->component, 0, xOffsetPos, yOffsetUsers + leaderBoardRows, maxUsernameChars);
		printlineInComponent(lb.users[i-1].username, &sc->component, 0, xOffsetUsername, yOffsetUsers + leaderBoardRows, maxUsernameChars);
		sprintf(buf,"%d", lb.users[i-1].bestScore);
		printlineInComponent(buf, &sc->component, 0, xOffsetScore, yOffsetUsers + leaderBoardRows, maxUsernameChars);
		sprintf(buf,"%d", 1<<lb.users[i-1].bestTile);
		printlineInComponent(buf, &sc->component, 0, xOffsetTile, yOffsetUsers + leaderBoardRows, maxUsernameChars);
		for (int j = 0; j < sc->cell_width; j++) {
			sc->component.pixels[yOffsetUsers + leaderBoardRows][j+1].styleCode = lb.users[i-1].bestTile;
		}
	}
}

void updateScoreBoard(BoardComponent* sc, BoardState curState, BoardState prevState, Component* digits, Component scoreText, int* numberDecomposition) {
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
	if (ranked) {
		getLeaderBoard(&lb);
		updateLeaderboard(&scoreBoard,lb);
	}
}

void updateBestScore(BoardState state) {
	if (state.score > bestScoreEver || state.maxTile > bestTileEver) {
		UserState bestState;
		strcpy(bestState.username,username);
		bestState.bestScore = state.score;
		bestState.bestTile = state.maxTile;
		updateBest(bestState);
	}
}

void loadUsername(char* username_) {
	uid_t ruid = getuid();
	struct passwd* pwd;
	pwd = getpwuid(ruid);
	strcpy(username_, pwd->pw_name);
	openDb();
	getLeaderBoard(&lb);
	// closeDb();
	sprintf(username_,"sass%d",lb.nOfUsers);
	// strcpy(username_, "sas9"); //provvisorio
}
void redrawScreen() {
	printf("\e[2J");
	copyScreen(&screen, &nextScreen);
	clearScreen(&screen);
	render(screen, nextScreen);
}

void loadPersonalBest() {
	bestScoreEver = 0;
	bestTileEver = 2;
	bool userExists = false;
	checkUserExists(username, &userExists);
	if (userExists) {
		getPersonalBest(username, &bestScoreEver, &bestTileEver);
		if (isStateValid(userState.state)) {
			copyBoardState(&userState.state, &cur_state);
		}
	}
}

int setUpSession(int argc, char** argv) {
	configure_terminal();
	signal(SIGINT, signal_handler);
	signal(SIGHUP, signal_handler);
	signal( SIGWINCH, redrawScreen);
	loadUsername(username);
	openDb();
	int max_n_cols = 10;
	int max_n_rows = 5;
	if(argc == 3){
		n_cols = atoi(argv[1]);
		n_rows =atoi(argv[2]);
		n_cols = n_cols > max_n_cols ? max_n_cols : n_cols;
		n_rows = n_rows > max_n_rows ? max_n_rows : n_rows;
	}else{
		n_cols = 4;
		n_rows = 4;
	}

	ranked = n_cols == 4 && n_rows == 4; //only 4x4 grids are ranked games



	if(newBoardState(&cur_state, n_rows, n_cols) || newBoardState(&prev_state, n_rows, n_cols)){
		printf("could not instantiate game\n");
		return -1;
	}

	//digits are used to display score in score component
	for (int i=0; i < 10; i++)
		newComponent(&digitsComponents[i],3,3);
	newComponent(&scoreText,3,16);
	newComponent(&infoText,8,75);
	loadDigits(&scoreText, digitsComponents);
	// getLeaderBoard(&lb);
	// updateLeaderboard(&scoreBoard,lb);
	newScreen(&screen, 3);
	newScreen(&nextScreen, 3);
	newBoardState(&userState.state, cur_state.n_rows, cur_state.n_cols);
	strcpy(userState.username, username);
	initGame(&cur_state);
	if (ranked) {
		bool userExists = false;
		checkUserExists(username, &userExists);
		if (userExists) {
			getSavedBoardState(&userState);
			if (isStateValid(userState.state)) {
				copyBoardState(&userState.state, &cur_state);
			}
		}
		else {
			userState.bestScore=0;
			userState.bestTile=2;
			copyBoardState(&cur_state, &userState.state);
			addUser(userState);
		}

	}
	copyBoardState(&cur_state, &prev_state);
	prev_state.score = 0;
	return 0;
}

void setUpGame() {
	// initGame(&cur_state);
	setupScreen(&screen, &nextScreen, &gameBoard, &scoreBoard, &infoBoard, n_rows, n_cols);
	for (int i = 0; i < n_rows; i++) {
		for (int j = 0; j < n_cols; j++) {
			updateGameCell(&gameBoard, i, j, cur_state.values[i][j],0);
		}
	}
	updateScoreBoard(&scoreBoard, cur_state, prev_state, digitsComponents, scoreText, numberDecomposition);
	updateInfoBoard(&infoBoard, infoText);
	loadPersonalBest();
	redrawScreen();
	render(screen, nextScreen);
	game_over = 0;
}

int gameStep() {
	input = read_input();
	if(input=='s'){
		return 1;
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
	return 0;
}

void tearDown() {

	if (ranked) {
		strcpy(userState.username, username);
		copyBoardState( &cur_state, &userState.state);
		updateBoardState(userState);
	}
	freeScreen(&screen);
	freeScreen(&nextScreen);
	freeBoardState(&cur_state);
	freeBoardState(&prev_state);
	// closeDb();
}

int main(int argc, char** argv){
	seed=time(NULL);
	srand(seed);
    rs = setUpSession(argc, argv);
	if (rs)
		return rs;
	// test();
	while(!exit_loop){
		setUpGame();
		// tearDown();
		do{
			if (gameStep()) {
				break;
			}
		}while(!game_over && !exit_loop);
		if (ranked) {
			updateBestScore(cur_state);
		}
		if (!exit_loop)
			initGame(&cur_state);

	}
	tearDown();
}