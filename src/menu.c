#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"


void displayMenu() {
    printf("=== Menu ===\n");
    printf("1. Play\n");
    printf("2. Load\n");
    printf("3. Create\n");
    printf("4. Quit\n");
    printf("Please enter your choice (%d-%d): ", PLAY, QUIT);
}

int init() {
    int choice;
    do {
        clearScreen();
        displayMenu();
    } while (!getChoice(&choice, PLAY, QUIT) && !feof(stdin));

    switch (choice) {
        case PLAY:
            return play();
            
        case LOAD:
            return load();
            
        case CREATE:
            return create();
            
        case QUIT:
            return quit();
    }
    return UNKNOWN_ERROR; 
}

int load() {
    return UNKNOWN_ERROR;
}

int create() {
    return UNKNOWN_ERROR;
}

int play() {
    return UNKNOWN_ERROR;
}

int quit() {
    return SUCCESS;
}