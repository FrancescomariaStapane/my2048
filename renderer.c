#include"2048game.h"
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
    Component component;
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
    TEST
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



int exit_no_memory(){
    printf("Insufficient memory for pixel matrix\n");
    exit(1);
}

int newComponent(Component * component, int height, int width){
    component -> height = height;
    component -> width = width;
    if(!(component->pixels_s = malloc(height * width * sizeof(Pixel))))
        exit_no_memory();
  
    if(!(component->pixels = malloc(component->height * sizeof(Pixel*))))
        exit_no_memory();

    for(int i = 0; i< component->height; i++){
        component->pixels[i] = &(component-> pixels_s[i*component->width]);
    }
}

void copyComponent(Component* dst, Component* src){
    int src_size = src -> height * src -> width;
    int dst_size = dst -> height * dst -> width;

    if(src_size != dst_size){
        freeComponent(dst);
        newComponent(dst, src -> height, src -> width);
    }
    memcpy(dst->pixels_s, src->pixels_s, src_size * sizeof(Pixel));
}

void copyPanel(Panel* dst, Panel* src){
    dst->offset_x = src->offset_x;
    dst->offset_y = src->offset_y;
    copyComponent(&(dst->component), &(src->component));
}

void copyScreen(Screen* dst, Screen* src){
    if(dst -> n_panels != src -> n_panels){
        freeScreen(dst);
        newScreen(dst, src->n_panels);
    }
    for (int i = 0; i < src->n_panels; i++){
        copyPanel(&(dst->panels[i]), &(src->panels[i]));
    }
}

int newBoardComponent(BoardComponent* boardComponent, int n_rows, int n_cols, int cell_height, int cell_width){
    int total_width = n_cols * (cell_width + 1) + 1;
    int total_height = n_rows * (cell_height + 1) + 1;
    boardComponent->n_cols = n_cols;
    boardComponent->n_rows = n_rows;
    boardComponent -> cell_height = cell_height;
    boardComponent -> cell_width = cell_width;
    newComponent(&(boardComponent -> component), total_height, total_width);
    drawGameGrid(boardComponent);
}


int getStyleCode(STYLE style){
    switch (style)
    {
    case GRID:
        return (-2);
        break;
    case TEST:
        return -(100);
        break;
    default:
        return (-1);
        break;
    }
}

void printComponent(Component component){
    for(int i= 0; i<component.height; i++){
        for(int j= 0; j<component.width; j++){
            renderPixel(component.pixels[i][j], i, j);
        }
        printf("\n");
    }
}

void freeComponent(Component* component){
    free(component->pixels_s);
    free(component->pixels);
}
void freeBoardComponent(BoardComponent* boardComponent){
    free(boardComponent->component.pixels_s);
    free(boardComponent->component.pixels);
}

void drawGameGrid(BoardComponent* board){
    for(int i = 0; i < board->component.height; i++){
         for(int j = 0; j < board->component.width; j++){
            if(!(j % (board->cell_width + 1)) || !(i % (board->cell_height +1))){
                strcpy(board->component.pixels[i][j].value,"█");
                board->component.pixels[i][j].styleCode = getStyleCode(GRID);
            }else{
                strcpy(board->component.pixels[i][j].value , " ");
                board->component.pixels[i][j].styleCode = 0;
            }
         }
     }
}

void newScreen(Screen* screen, int n_panels){
    screen->panels = malloc(n_panels * sizeof(Panel));
    screen->n_panels = n_panels;
    for( int i = 0; i< n_panels; i++){
        screen -> panels[i].offset_x = 0;
        screen -> panels[i].offset_y = 0;
        screen -> panels[i].component.height = 0;
        screen -> panels[i].component.width = 0;
        screen -> panels[i].component.pixels_s = NULL;
        screen -> panels[i].component.pixels = NULL;
    }
}

void clearScreen(Screen* screen){
     for(int i = 0; i < screen -> n_panels; i++){
        int n_pixels = screen -> panels[i].component.height * screen -> panels[i].component.width;
        for (int j = 0; j < n_pixels; j++){
            strcpy(screen -> panels[i].component.pixels_s[j].value ,"");
            screen -> panels[i].component.pixels_s[j].styleCode = getStyleCode(DEFAULT);
        }
    }
}
void freeScreen(Screen* screen){
    for(int i = 0; i < screen -> n_panels; i++){
        freeComponent(&(screen->panels[i].component));
    }
    free(screen->panels);
}




void renderPixel(Pixel pixel, int x, int y){
    printf("\e[%d;%dH", x + 1, y + 1); //move cursor
    printStyledPixel(pixel);
}

//screens are dissimilar if their geometries differ, aka if contain different amount of panels or corresponding panels have different dimensions and/or offsets
bool areScreensDissimilar(Screen sc1, Screen sc2){
    if(sc1.n_panels != sc2.n_panels)
        return true;
    
    
    for(int i = 0; i < sc1.n_panels; i++){
        if(sc1.panels[i].offset_x != sc2.panels[i].offset_x)
            return true;
        if(sc1.panels[i].offset_y != sc2.panels[i].offset_y)
            return true;
        if(sc1.panels[i].component.height != sc2.panels[i].component.height)
            return true;
        if(sc1.panels[i].component.width != sc2.panels[i].component.width)
            return true;
    }
    return false;
}

void printStyledPixel(Pixel pixel){
    char* formatStart;
    char*formatEnd;
    // printf("%d",pixel.styleCode);
    switch (pixel.styleCode){
    case 0:
        //  BG: #FFF3F3
        formatStart = "\x1B[48;5;231m";
        formatEnd = "\x1B[49m";
        break;
        
    case -1: //default
        formatStart = "";
        formatEnd = "";
        break;
    case -2:
        // FG: #988E8E
        formatStart = "\x1B[38;5;145m";
        formatEnd = "\x1B[39m";
        break;
    case -100: //test
        formatStart="\x1B[38;5;160m\x1B[48;5;78m\x1B[3m\x1B[9m";
        formatEnd = "\x1B[29m\x1B[23m\x1B[49m\x1B[39m";
        break;
    default:
        formatStart = "";
        formatEnd = "";
        break;
    }
    printf("%s%s%s",formatStart,pixel.value,formatEnd);
}
int getXOffsetRightOfPanel(Panel panel){
    return panel.offset_x + panel.component.width;
}
int getYOffsetDownPanel(Panel panel){
    return panel.offset_y + panel.component.height;
}
void printScreen(Screen screen){
    for(int k = 0; k< screen.n_panels; k++){
        for (int i = 0; i < screen.panels[k].component.height; i++){
            for(int j = 0; j < screen.panels[k].component.width; j++){
                Pixel* p = &(screen.panels[k].component.pixels[i][j]);
                renderPixel(*p, i + screen.panels[k].offset_y, j + screen.panels[k].offset_x);
            }
        }
    }
}

void render(Screen cur, Screen nxt){
    bool areDissimilar = false;
    if(areScreensDissimilar(cur, nxt)){
        areDissimilar = true;
        newScreen(&cur,0);
        copyScreen(&cur, &nxt);
        clearScreen(&cur);
    }
    for(int k = 0; k< nxt.n_panels; k++){
        for (int i = 0; i < nxt.panels[k].component.height; i++){
            for(int j = 0; j < nxt.panels[k].component.width; j++){
                Pixel* p_cur = &(cur.panels[k].component.pixels[i][j]);
                Pixel* p_nxt = &(nxt.panels[k].component.pixels[i][j]);
                if(!strcmp(p_cur->value, p_nxt->value) || p_cur ->styleCode != p_nxt ->styleCode){
                    renderPixel(*p_nxt, i + nxt.panels[k].offset_y, j + nxt.panels[k].offset_x);
                }
            }
        }
    }
    if(areDissimilar){
        freeScreen(&cur);
    }  
}



int main(){
    BoardComponent bc1;
    BoardComponent bc2;
    BoardComponent bc3;
    // Component cp;
    
    newBoardComponent(&bc1,2,3,7,15);
    newBoardComponent(&bc2,2,4,7,15);
    newBoardComponent(&bc3,1,1,7,15);
    
    Screen screen;
    Screen neextScreen;
    newScreen(&screen, 3);
    newScreen(&neextScreen, 1);
    screen.panels[0].component = bc1.component;
    screen.panels[1].component = bc2.component;
    screen.panels[2].component = bc3.component;

    screen.panels[0].offset_x = 3;
    screen.panels[0].offset_y = 3;

    //todo funzione  int offset_after horizontally/ vertically (Panel panel, additional offset x, y)
    screen.panels[1].offset_x = getXOffsetRightOfPanel(screen.panels[0]) +2;
    screen.panels[1].offset_y = screen.panels[0].offset_y;

    screen.panels[2].offset_x = screen.panels[0].offset_x;
    screen.panels[2].offset_y = getYOffsetDownPanel(screen.panels[0]) +3;


    copyScreen(&neextScreen, &screen);
    clearScreen(&screen);
    // printScreen(neextScreen);
    render(screen, neextScreen);

    // drawGameGrid(&bc1);
    // drawGameGrid(&bc2);
    // copyComponent(&(bc2.component), &(bc1.component));
    // freeComponent(&(bc.component));
    // printComponent(bc2.component);
    // Pixel pixel1;
    // pixel1.value = " ";
    // pixel1.styleCode = 0;
    // // renderPixel(pixel1, 1,1);
    // // pixel1.value = "ciao";
    // char*        formatStart;
    // char*    formatEnd;
    // formatStart = "\x1B[48;5;231m";
    // formatEnd = "\x1B[49m";
    // printf("%s%s%s",formatStart,pixel1.value,formatEnd);
}