#include<termios.h>
#include<stdio.h>
#include<unistd.h>
#include<time.h>
#include<string.h>
#include<signal.h>
#include<stdlib.h>


static     struct termios old_termios, new_termios;
// static int exit_loop = 0;

// void reset_terminal(struct termios* old_termios){
void reset_terminal(){
    printf("\e[m"); //reset color changes
    printf("\e[?25h"); //show cursor
    printf("\033[2J\033[?47l\0338"); //return to saved cursor and original buffer

    // printf("\e[1;1H"); // Move to upper left corner
    fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
}

void configure_terminal(){
    printf("\0337\033[?47h"); //save current cursor and switch to secondary buffer
    printf("\e[2J"); // Clearing the screen

    printf("\e[1;1H"); // Move to upper left corner
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO); //turn of echo and canonical mode
    new_termios.c_cc[VMIN] = 0; // read can return instantly, there's no mimimum byte to be read
    new_termios.c_cc[VTIME] = 0; // read can return instantly, no waiting time

    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios); //apply changes now

    printf("\e[?25l");
    atexit(reset_terminal); // function called when exiting normally (not by interrupt)
}


char read_key(char* buf, int k){

    // printf(" %c\n",buf[k]);
    if(buf[k] == '\033' && buf[k+1] == '['){
        switch (buf[k+2]){
        case 'A': return 'u';
        case 'B': return 'd';
        case 'C': return 'r';
        case 'D': return 'l';
        }
    }
    if(buf[k] == 'u'){
        return 'c';
    }
    if(buf[k] == 'R'){
        return 's'; //s for start over
    }
    return '0';
}


char read_input(){
    char buf[4096]; //max input buffer
    int n = read(STDIN_FILENO, buf, sizeof(buf));
    char final_key='0';
    for(int k = 0; k <= n-1; k+=1){
        char key = read_key(buf, k);
        // printf("key: %c\n",key);
        if(key == '0') continue;
        final_key = key;
        // printf("final key: %c\n",final_key);
    }
    return final_key;
}

void print_key(char key){
    if(key == 'l' || key == 'd' || key == 'u' || key == 'r' || key == 's' || key == 'c' ){
        printf("%c\n",key);
    }
}



// int main(){
//     struct termios old_termios, new_termios;
//     configure_terminal();

//     signal(SIGINT, signal_handler);

//     struct  timespec req ={};
//     struct  timespec rem ={};

//     while (!exit_loop)
//     {
//         char key = read_input();
//         print_key(key);
//         req.tv_nsec = 0.1 * 100000000; // 0.01 seconds
//         nanosleep(&req, &rem);
//     }
// }