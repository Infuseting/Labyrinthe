#include <utils.h>
#include <stdio.h>
#include <stdlib.h>




bool getChoice(int *choice, int min, int max) {
    int input;
    if (scanf("%d", &input) != 1) {
        clearInputBuffer();
        return false;
    }
    clearInputBuffer();
    if (input < min || input > max) {
        return false;
    }
    *choice = input;
    return true;
}

/*
    @brief This function clears the input buffer to remove any unwanted characters.
*/
void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

void clearScreen() {
    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #else
        system("clear");
    #endif
}