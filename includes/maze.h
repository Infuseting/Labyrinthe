#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>

/*
    ============================================================================
    Constants and Symbols (Step 1 & Step 2)
    ============================================================================
*/

/**
 * @brief Default fixed dimensions for the perfect maze (Step 1).
 */
#define DEFAULT_HEIGHT 11
#define DEFAULT_WIDTH  25

/**
 * @brief Minimum valid dimensions for dynamic mazes (must be odd numbers >= 3).
 */
#define MIN_HEIGHT 3
#define MIN_WIDTH  3

/**
 * @brief Character symbols used for rendering and .cfg files.
 */
#define SYMBOL_WALL   '#'
#define SYMBOL_PATH   ' '
#define SYMBOL_PLAYER 'o'
#define SYMBOL_EXIT   '-'

/**
 * @brief Controls for player movement (Step 2).
 */
#define MOVE_UP    'z'
#define MOVE_LEFT  'q'
#define MOVE_DOWN  's'
#define MOVE_RIGHT 'd'

/*
    ============================================================================
    Structures and Enumerations
    ============================================================================
*/

/**
 * @brief Enumeration defining the different types of tiles in the maze grid.
 */
typedef enum TileType {
    WALL = 0,
    PATH = 1,
    EXIT = 2,
    PLAYER = 3
} TileType;

/**
 * @brief Represents 2D coordinates in the maze (x = column, y = row).
 */
typedef struct Coords {
    int x;
    int y;
} Coords;

/**
 * @brief Represents a maze with dynamic grid, dimensions, and positions.
 */
typedef struct Maze {
    int height;              /**< Height of the maze (number of rows) */
    int width;               /**< Width of the maze (number of columns) */
    Coords start;            /**< Coordinates of the entrance (start position, (1, 1)) */
    Coords end;              /**< Coordinates of the exit door ('-') */
    Coords playerPosition;   /**< Current position of the player (x, y) */
    TileType** tiles;        /**< 2D dynamically allocated array of tiles */
} Maze;

/*
    ============================================================================
    Function Prototypes
    ============================================================================
*/

/* --- Dimension Validation --- */

/**
 * @brief Checks if the given dimensions are valid for a maze (positive, odd numbers >= 3).
 * @param height Height to check.
 * @param width Width to check.
 * @return true if dimensions are valid, false otherwise.
 */
bool areValidDimensions(int height, int width);

/* --- Memory Management (Allocation & Deallocation) --- */

/**
 * @brief Allocates and initializes a new Maze structure and its 2D grid.
 * @param height Height of the maze (must be an odd integer >= 3).
 * @param width Width of the maze (must be an odd integer >= 3).
 * @return Pointer to the newly allocated Maze structure, or NULL on allocation failure.
 */
Maze* createMaze(int height, int width);

/**
 * @brief Frees all memory allocated for a maze (its grid and structure).
 * @param maze Pointer to the maze to free. Safe to call with NULL.
 */
void freeMaze(Maze* maze);

/**
 * @brief Allocates the 2D tiles grid of an existing Maze structure.
 * @param maze Pointer to the Maze structure.
 * @param height Height of the grid.
 * @param width Width of the grid.
 * @return true if grid allocation succeeded, false otherwise.
 */
bool initMaze(Maze* maze, int height, int width);

/**
 * @brief Frees only the 2D tiles grid of a Maze structure without freeing the structure itself.
 * @param maze Pointer to the maze whose grid will be freed.
 */
void freeMazeGrid(Maze* maze);

/* --- Maze Generation --- */

/**
 * @brief Generates a perfect maze of the specified height and width.
 * Initializes tiles, creates paths using the generation algorithm, sets entrance at (1, 1),
 * exit at bottom-right, and positions the player at the start.
 * @param maze Pointer to the maze structure.
 * @param height Height of the maze.
 * @param width Width of the maze.
 * @param seed Random seed for generation (0 uses a time-based seed, non-zero for deterministic tests).
 */
void generateMaze(Maze* maze, int height, int width, int seed);

/* --- Player Movement and Game Logic --- */

/**
 * @brief Checks if a move to the specified coordinates is valid (within bounds and not a wall).
 * @param maze Pointer to the maze.
 * @param x Column coordinate to test.
 * @param y Row coordinate to test.
 * @return true if the move is within bounds and walkable, false otherwise.
 */
bool isValidMove(const Maze* maze, int x, int y);

/**
 * @brief Moves the player by the specified offset (dx, dy).
 * @param maze Pointer to the maze.
 * @param dx Horizontal displacement (-1 for left, 1 for right, 0 otherwise).
 * @param dy Vertical displacement (-1 for up, 1 for down, 0 otherwise).
 * @return true if movement succeeded, false if blocked by a wall or out of bounds.
 */
bool movePlayer(Maze* maze, int dx, int dy);

/**
 * @brief Moves the player based on a directional character ('z', 'q', 's', 'd').
 * @param maze Pointer to the maze.
 * @param direction Direction character ('z', 'q', 's', 'd' or uppercase).
 * @return true if movement succeeded, false if blocked or if direction is invalid.
 */
bool movePlayerDirection(Maze* maze, char direction);

/**
 * @brief Checks if the maze has been solved by comparing the player's position with the exit position.
 * @param maze Pointer to the maze.
 * @return true if playerPosition equals end position, false otherwise.
 */
bool isMazeSolved(const Maze* maze);

/* --- Display and Character Representation --- */

/**
 * @brief Returns the character representation of the tile at the specified coordinates.
 * Returns SYMBOL_PLAYER ('o') if the player is currently at these coordinates.
 * @param maze Pointer to the maze.
 * @param coords Coordinates of the tile.
 * @return Character symbol representing the tile or player ('#', ' ', 'o', '-').
 */
char getChar(const Maze* maze, Coords coords);

/**
 * @brief Prints the maze grid to the standard output console.
 * @param maze Pointer to the maze to display.
 */
void displayMaze(const Maze* maze);

/* --- File Management (.cfg) --- */

/**
 * @brief Saves the maze into a configuration file (.cfg).
 * @param maze Pointer to the maze to save.
 * @param filename File path or name where the maze will be stored.
 * @return true if saving succeeded, false otherwise.
 */
bool saveMaze(const Maze* maze, const char* filename);

/**
 * @brief Loads a maze from a configuration file (.cfg).
 * @param filename File path or name of the .cfg file to load.
 * @return Pointer to the newly allocated Maze loaded from the file, or NULL on error.
 */
Maze* loadMaze(const char* filename);

#endif /* MAZE_H */
