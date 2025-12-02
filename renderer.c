#include "renderer.h"
#include<unistd.h>

int exit_no_memory(){
    printf("Insufficient memory for pixel matrix\n");
    exit(1);
}

int newComponent(Component * component, int height, int width){
    component -> height = height;
    component -> width = width;
    if(!((component->pixels_s = malloc(height * width * sizeof(Pixel)))))
        exit_no_memory();
  
    if(!((component->pixels = malloc(component->height * sizeof(Pixel*)))))
        exit_no_memory();

    for(int i = 0; i< component->height; i++){
        component->pixels[i] = &(component-> pixels_s[i*component->width]);
    }
    return 0;
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
    copyComponent((dst->component), (src->component));
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
            return -2;
        case TEST:
            return -100;
        case OVER_4096:
            return -3;

        default:
            return (-1);
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
    if(!component){
        free(component->pixels_s);
        free(component->pixels);
    }
}
void freeBoardComponent(BoardComponent* boardComponent){
    if(!boardComponent){
        free(boardComponent->component.pixels_s);
        free(boardComponent->component.pixels);
    }
}

void drawGameGrid(BoardComponent* board){
    for(int i = 0; i < board->component.height; i++){
         for(int j = 0; j < board->component.width; j++){
            if(!(j % (board->cell_width + 1)) || !(i % (board->cell_height +1))){
                strcpy(board->component.pixels[i][j].value," ");
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
        screen -> panels[i].component = malloc(sizeof(Component));
        screen -> panels[i].component -> height = 0;
        screen -> panels[i].component -> width = 0;
        screen -> panels[i].component -> pixels_s = NULL;
        screen -> panels[i].component -> pixels = NULL;
    }
}

void clearScreen(Screen* screen){
     for(int i = 0; i < screen -> n_panels; i++){
        int n_pixels = screen -> panels[i].component -> height * screen -> panels[i].component -> width;
        for (int j = 0; j < n_pixels; j++){
            strcpy(screen -> panels[i].component -> pixels_s[j].value ,"");
            screen -> panels[i].component -> pixels_s[j].styleCode = getStyleCode(DEFAULT);
        }
    }
}
void freeScreen(Screen* screen){
    if(!screen){
        for(int i = 0; i < screen -> n_panels; i++){
            freeComponent((screen->panels[i].component));
        }
        free(screen->panels);
    }
}




void renderPixel(Pixel pixel, int x, int y){
    printf("\e[%d;%dH", x + 1, y + 1); //move cursor
    printStyledPixel(pixel);
    fflush(stdout);

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
        if(sc1.panels[i].component -> height != sc2.panels[i].component -> height)
            return true;
        if(sc1.panels[i].component -> width != sc2.panels[i].component -> width)
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
            formatStart = "\x1B[38;5;16m\x1B[48;5;247m";
            formatEnd = "\x1B[39m\x1B[49m";
            break;
        case 1:
            formatStart = "\x1B[38;5;16m\x1B[48;5;231m\x1B[1m";
            formatEnd = "\x1B[22m\x1B[49m\x1B[39m";
            break;
        case 2:
            formatStart = "\x1B[38;5;16m\x1B[48;2;235;216;182m\x1B[1m";
            formatEnd = "\x1B[22m\x1B[49m\x1B[39m";
            break;
        case 3:
            formatStart = "\x1B[38;5;16m\x1B[48;2;249;155;107m\x1B[1m";
            formatEnd = "\x1B[22m\x1B[49m\x1B[39m";
            break;
        case 4:
            formatStart = "\x1B[38;5;16m\x1B[48;2;244;122;60m\x1B[1m";
            formatEnd = "\x1B[22m\x1B[49m\x1B[39m";
            break;
        case 5:
            formatStart = "\x1B[38;5;16m\x1B[48;2;237;90;56m\x1B[1m";
            formatEnd = "\x1B[22m\x1B[49m\x1B[39m";
            break;
        case 6:
            formatStart = "\x1B[38;5;16m\x1B[48;2;215;51;12m\x1B[1m";
            formatEnd = "\x1B[22m\x1B[49m\x1B[39m";
            break;
        case 7:
            formatStart = "\x1B[38;5;16m\x1B[48;2;236;204;113m\x1B[1m";
            formatEnd = "\x1B[22m\x1B[49m\x1B[39m";
            break;
        case 8:
            formatStart = "\x1B[38;5;16m\x1B[48;2;234;196;88m\x1B[1m";
            formatEnd = "\x1B[22m\x1B[49m\x1B[39m";
            break;
        case 9:
            formatStart = "\x1B[38;5;16m\x1B[48;2;252;201;56m\x1B[1m";
            formatEnd = "\x1B[22m\x1B[49m\x1B[39m";
            break;
        case 10:
            formatStart = "\x1B[38;5;16m\x1B[48;2;242;187;28m\x1B[1m";
            formatEnd = "\x1B[22m\x1B[49m\x1B[39m";
            break;
        case 11:
            formatStart = "\x1B[38;5;16m\x1B[48;2;255;189;0m\x1B[1m";
            formatEnd = "\x1B[22m\x1B[49m\x1B[39m";
            break;





        case -1: //default
            formatStart = "";
            formatEnd = "";
            break;
        case -2: //GRID
            // FG: #988E8E
            formatStart = "\x1B[48;5;237m";
            formatEnd = "\x1B[49m";
            break;
        case -3: //OVER_2046
            formatStart = "\x1B[38;5;16m\x1B[48;5;231m";
            formatEnd = "\x1B[7m\x1B[49m\x1B[39m";;
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
    return panel.offset_x + panel.component -> width;
}
int getYOffsetDownPanel(Panel panel){
    return panel.offset_y + panel.component -> height;
}
void printScreen(Screen screen){
    for(int k = 0; k< screen.n_panels; k++){
        int sas;
        for (int i = 0; i < screen.panels[k].component -> height; i++){
            int mas;
            for(int j = 0; j < screen.panels[k].component -> width; j++){
                Pixel* p = &(screen.panels[k].component -> pixels[i][j]);
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
    int i, j, k = 0;
    for( k = 0; k< nxt.n_panels; k++){
        for ( i = 0; i < nxt.panels[k].component -> height; i++){
            for( j = 0; j < nxt.panels[k].component -> width; j++){
                Pixel* p_cur = &(cur.panels[k].component -> pixels[i][j]);
                Pixel* p_nxt = &(nxt.panels[k].component -> pixels[i][j]);
                // if (k == 2 && i == )
                if(strcmp(p_cur->value, p_nxt->value) || p_cur ->styleCode != p_nxt ->styleCode){
                    renderPixel(*p_nxt, i + nxt.panels[k].offset_y, j + nxt.panels[k].offset_x);
                }
            }
        }
    }
    if(areDissimilar){
        freeScreen(&cur);
    }  
}

int getPosOfBoardComponentCell(BoardComponent bc, int i, int j, int *x, int* y){
    if(i < 0 || j < 0 || i > bc.n_rows || j > bc.n_cols){
        return -1;
    }
    *x = (bc.cell_width + 1)*j + 1;
    *y = (bc.cell_height + 1)*i + 1;
    return 0;
}

int readCellFromFile(char* fileName, Component* cell, int cellHeight, int cellWidth) {
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(fileName, "r");
    if (fp == NULL)
        return -1;
    int i= 0;
    int j = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        char* delimiter = "|";
        char* token = strtok(line,delimiter);
        j = 0;
        while (token != NULL) {
            if (i > cellHeight || j > cellWidth) {
                fclose(fp);
                if (line)
                    free(line);
                return -1;
            }
            strcpy(cell->pixels[i][j].value, token);
            token = strtok(NULL, delimiter);
            j++;
        }
        i++;
    }
    fclose(fp);
    if (line)
        free(line);
    if (i != cellHeight || j != cellWidth) {
        return -1;
    }
    return 0;
}
void styleAllInComponent(Component* component, int styleCode) {
    for (int i_ = 0; i_< component->height; i_++) {
        for (int j_ = 0; j_ < component->width; j_++) {
            component->pixels[i_][j_].styleCode = styleCode;
        }
    }
}
int loadInfo(Component* ic) {
    char* workingDir = "."; // todo da cambiare
    char* fontDir = "/resources/info/";
    char* fileName = malloc(sizeof(char) * (strlen(fontDir) + strlen(workingDir) + 10));

    sprintf(fileName, "%s%sinfo.txt",workingDir,fontDir);
    readCellFromFile(fileName, ic, ic->height, ic->width);
    free(fileName);

}
int load_digits(Component* scoreText, Component* digits) {
    char* workingDir = "."; // todo da cambiare
    char* fontDir = "/resources/score/";
    char* fileName = malloc(sizeof(char) * (strlen(fontDir) + strlen(workingDir) + 10));
    for (int i = 0; i< 10; i++) {
        sprintf(fileName, "%s%s%d.txt",workingDir,fontDir,i);
        newComponent(digits + i,digits[i].height,digits[i].width);
        readCellFromFile(fileName, &digits[i], 3, 3);
    }
    sprintf(fileName, "%s%sscore.txt",workingDir,fontDir);
    readCellFromFile(fileName, scoreText, scoreText->height, scoreText->width);
    for (int i = 0; i< 10; i++)
        styleAllInComponent(&digits[i], 0);
    styleAllInComponent(scoreText, 0);

    free(fileName);
    return 0;
}
int copySubComponentInComponent(Component subComponent, Component* component, int offsetX, int offsetY) {
    if (subComponent.width + offsetX >= component->width || subComponent.height + offsetY >= component->height)
        return -1;
    for (int i = 0; i< subComponent.height; i++) {
        for (int j = 0; j < subComponent.width; j++) {
            strcpy(component->pixels[i + offsetY][j + offsetX].value, subComponent.pixels[i][j].value);
            component->pixels[i + offsetY][j + offsetX].styleCode = subComponent.pixels[i][j].styleCode;
        }
    }
    return  0;
}
int getXOffsetToCenterComponent(int outerWidth, int innerWidth) {
    return (outerWidth -innerWidth)/2;
}

void decomposeNumber(int number, int* n_digits, int* array) {
    int i = 0;
    while (number > 0) {
        array[i] = number % 10;
        number /= 10;
        i++;
    }
    *n_digits = i;
}


// int main(){

// }