#include"2048game.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
typedef struct BoardScreen{
    int total_height;
    int total_width;
    int cell_height;
    int cell_width;
    int n_cell_rows;
    int n_cell_cols;
    int n_rows;
    int n_cols;
    char** pixel_values_s; // storage for screen chars
    char*** pixel_values; //matrix for screen chars
    
    // char** pixel_formats_s; //storage for pixel formats
    // char*** pixel_formats; //matrix for pixel formats

}BoardScreen;

// typedef struct GameState{
//     BoardScreen current;
//     BoardScreen prev;
//     board_state state;
// }GameState;


int new_board_screen(BoardScreen* screen, int n_rows, int n_cols, int cell_height, int cell_width);
void free_board_screen(BoardScreen* screen);
void copy_board_screen(BoardScreen* screen_dst, BoardScreen* screen_src);
void print_screen(BoardScreen bs);
// void new_game_state(GameState* state, int n_rows, int n_cols, int cell_height, int cell_width);
// void free_game_state(GameState* state);



int new_board_screen(BoardScreen* screen, int n_rows, int n_cols, int cell_height, int cell_width){
    screen-> total_width = n_cols * (cell_width + 1) + 1;
    screen-> total_height = n_rows * (cell_height + 1) + 1;
    screen->n_cols = n_cols;
    screen->n_rows = n_rows;
    screen -> cell_height = cell_height;
    screen -> cell_width = cell_width;

    int bytes_per_graphene = 8;
    if(!(screen->pixel_values_s = malloc(sizeof(char*) * screen->total_height * screen->total_width))){
        printf("Insufficient memory for pixel matrix\n");
        return 1;
    }
    if(!(screen->pixel_values = malloc(screen->total_height * sizeof(char**)))){
        printf("Insufficient memory for pixel matrix\n");
        return 1;
    }
    for (int i = 0; i< screen->total_height * screen->total_width; i++){
        if(!(screen->pixel_values_s[i] = malloc(sizeof(char) * bytes_per_graphene))){
            printf("Insufficient memory for pixel matrix\n");
            return 1;
    }
    }
    for(int i = 0; i< screen->total_height; i++){
        screen->pixel_values[i] = &(screen-> pixel_values_s[i*screen->total_width]);
    }
}

void print_screen(BoardScreen bs){
     for(int i= 0; i<bs.total_height; i++){
         for(int j= 0; j<bs.total_width; j++){
            printf("%s", bs.pixel_values[i][j]);
         }
         printf("\n");
     }
}

int main(){
    BoardScreen bs;
    new_board_screen(&bs, 2,3,4,5);
    // printf("%d", bs.total_width);
    for(int i= 0; i<bs.total_height; i++){
         for(int j= 0; j<bs.total_width; j++){
            // bs.pixel_values[i][j] = "sas";
            if(!(j % (bs.cell_width + 1)) || !(i % (bs.cell_height +1))){
                bs.pixel_values[i][j] = "█";
            }else{
                bs.pixel_values[i][j] = " ";
            }
         }
     }
     print_screen(bs);
}



// void new_game_state(game_state*)

