/* 
    @brief This header file defines the menu options and function prototypes for the menu system.
*/
typedef enum MenuOption {
    PLAY = 1,
    LOAD = 2,
    CREATE = 3,
    QUIT = 4,
} MenuOption;

/* 
    @brief Function prototypes for the menu system. These functions handle displaying the menu, initializing the menu, and executing the selected menu option.
*/
int init();
/* 
    @brief Displays the menu options to the user.
*/
void displayMenu();
/*
    @brief Executes the "Play" option of the menu.
*/
int play();
/*
    @brief Executes the "Load" option of the menu.
*/
int load();
/*
    @brief Executes the "Create" option of the menu.
*/
int create();
/*
    @brief Executes the "Quit" option of the menu.
*/
int quit();