#include <stdbool.h>

typedef enum ErrorCode {
    SUCCESS = 0,
    UNKNOWN_ERROR = 1,
} ErrorCode;


/*
    @brief This function prompts the user for a choice between min and max (inclusive) and stores the result in the provided pointer.
    @param choice Pointer to an integer where the user's choice will be stored.
    @param min The minimum valid choice.
    @param max The maximum valid choice.


*/
bool getChoice(int *choice, int min, int max);

/*
    @brief This function clears the input buffer to remove any unwanted characters.
*/
void clearInputBuffer();

/*
    @brief This function clears the console screen. It uses system-specific commands to achieve this.
*/
void clearScreen();