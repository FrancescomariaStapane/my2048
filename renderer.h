
#ifndef LIBRENDERER_RENDERER_H
#define LIBRENDERER_RENDERER_H

#include"game.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#define PIXEL_BYTES 16

typedef struct Pixel{
    int styleCode;
    char value[PIXEL_BYTES]; 
}Pixel;

typedef  Pixel** PixelMatrix;
typedef Pixel* PixelStorage;

typedef struct Component{
    int height;
    int width;
    PixelStorage pixels_s; // storage for screen chars
    PixelMatrix pixels; //matrix for screen chars
}Component;

typedef struct BoardComponent{
    Component component;
    int cell_height;
    int cell_width;
    int n_cell_rows;
    int n_cell_cols;
    int n_rows;
    int n_cols;
}BoardComponent;

typedef struct Panel{
    int offset_x;
    int offset_y;
    Component* component;
}Panel;

typedef struct Screen{
    Panel* panels;
    int n_panels;
}Screen;


typedef struct GameState{
    Screen current;
    Screen prev;
    board_state state;
}GameState;

typedef enum STYLE{
    GRID,
    DEFAULT,
    TEST,
    OVER_4096
}STYLE;

int newComponent(Component * component, int height, int width);
int newBoardComponent(BoardComponent* boardComponent, int n_rows, int n_cols, int cell_height, int cell_width);
void freeComponent(Component* component);
void freeBoardComponent(BoardComponent* boardComponent);
void copyComponent(Component* dst, Component* src);
void copyScreen(Screen* dst, Screen* src);
void render(Screen cur, Screen nxt);
void printScreen(Screen screen);
void printComponent(Component component);
void drawGameGrid(BoardComponent* board);
int getStyleCode(STYLE style);
char* getStyle(int styleCode);
void renderPixel(Pixel pixel, int x, int y);
void printStyledPixel(Pixel pixel);
void newScreen(Screen* screen, int n_panels);
void freeScreen(Screen* screen);
void copyPanel(Panel* dst, Panel* src);
void initScreen(Screen* srceen);
void clearScreen(Screen* sceen);
bool areScreensDissimilar(Screen sc1, Screen sc2);
int getXOffsetRightOfPanel(Panel panel);
int getYOffsetDownPanel(Panel panel);
int getPosOfBoardComponentCell(BoardComponent bc, int i, int j, int *x, int* y);
int readCellFromFile(char* file, Component* cell, int cellHeight, int cellWidth );
int copySubComponentInComponent(Component subComponent, Component* component, int offsetX, int offsetY);
int load_digits(Component* scoreText, Component* digits);
void styleAllInComponent(Component* component, int styleCode);
int getXOffsetToCenterComponent(int outerWidth, int innerWidth);
void decomposeNumber(int number, int* n_digits, int* array);
int loadInfo(Component* ic);

#endif