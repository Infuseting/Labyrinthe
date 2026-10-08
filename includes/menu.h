
typedef enum MenuOption {
    PLAY = 1,
    LOAD = 2,
    CREATE = 3,
    QUIT = 4,
} MenuOption;


int init();
void displayMenu();
int play();
int load();
int create();
int quit();