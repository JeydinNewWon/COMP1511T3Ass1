// Program name
//
// This program was written by [your name] (z5555555)
// on [date]
//
// TODO: Description of program

#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  CONSTANTS  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Provided constants
#define SIZE        9
#define TRUE        1
#define FALSE       0
#define MAX_LIVES   3

// TODO: you may choose to add additional #defines here.

// Provided Enums
enum tile_type {LILLYPAD, BANK, WATER, TURTLE, LOG};

// custom Enum
enum bug_move_dir {LEFT, RIGHT, NONE};
enum available_space {LEFT_SPACE, RIGHT_SPACE, BOTH_SPACES, NO_SPACE};

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  STRUCTS  //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Provided structs
struct board_tile {
    enum tile_type type; // The type of piece it is (water, bank, etc.)
    int occupied;        // TRUE or FALSE based on if Frogger is there.
    int has_bug;
    enum bug_move_dir bug_dir;
    int bug_moved;
};

struct frogger_data {
    int row;
    int col;
    int lives;
    int win;
};
////////////////////////////////////////////////////////////////////////////////
/////////////////////////////  FUNCTION PROTOTYPES  ////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// TODO: Your function prototypes here

// Prints out the current state of the board.
void print_board(struct board_tile board[SIZE][SIZE]);
char type_to_char(enum tile_type type);
void setup_board(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger);
void add_turtles(struct board_tile board[SIZE][SIZE], int turtle_num);
void command_handler(struct board_tile board[SIZE][SIZE], char command, struct frogger_data *frogger);
int check_for_turtle_on_row(struct board_tile board[SIZE][SIZE], int row);
int validate_input(int row_or_col);
int check_for_frogger(struct board_tile board[SIZE][SIZE], int row);
int check_for_frogger_on_log(struct board_tile board[SIZE][SIZE], int row, int log_start_col);
int find_log_start_col(struct board_tile board[SIZE][SIZE], int row, int col);
void print_lives_status(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger);
void check_game_win(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger);
int execute_move(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger, int row_mod, int col_mod);
void return_frogger_start(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger);
void move_bugs(struct board_tile board[SIZE][SIZE]);
void move_single_bug(struct board_tile board[SIZE][SIZE], int row, int col);
enum available_space check_bug_space(struct board_tile board[SIZE][SIZE], int row, int col);
int is_available_for_bug(struct board_tile board[SIZE][SIZE], int row, int col, int col_mod);
void execute_bug_move(struct board_tile board[SIZE][SIZE], int row, int col, int col_mod);



// Commands
void add_log(struct board_tile board[SIZE][SIZE]);
void clear(struct board_tile board[SIZE][SIZE]);
void delete_log(struct board_tile board[SIZE][SIZE]);
void move_frogger(struct board_tile board[SIZE][SIZE], char direction, struct frogger_data *frogger);
void add_bug(struct board_tile board[SIZE][SIZE]);


////////////////////////////////////////////////////////////////////////////////
//////////////////////////  FUNCTION IMPLEMENTATIONS  //////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(void) {

    printf("Welcome to CSE Frogger!\n");
    struct board_tile game_board[SIZE][SIZE];

    struct frogger_data frogger;

    setup_board(game_board, &frogger);

    // Read user input and place turtles.
    printf("How many turtles? ");

    int turtle_num;
    scanf(" %d", &turtle_num);


    add_turtles(game_board, turtle_num);

    // Start the game and print out the gameboard.
    printf("Game Started\n");
    print_board(game_board);

    // TODO (Stage 1.3): Create a command loop, to read and execute commands!
    char command;
    printf("Enter command: ");
    while (scanf(" %c", &command) == 1) {
        
        command_handler(game_board, command, &frogger);

        if (frogger.win || frogger.lives <= 0) break;

        print_board(game_board);
        printf("Enter command: ");
    }

    if (frogger.win) {
        print_board(game_board);
        printf("\nWahoo!! You Won!\n\n");
    }

    printf("Thank you for playing CSE Frogger!\n");
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////// ADDITIONAL FUNCTIONS /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// TODO: Add more functions here!

////////////////////////////////// COMMANDS ///////////////////////////////////

// Adds a log to the board. Takes an input of row, start_col and end_col. 
// It adds the logs inclusive. 
void add_log(struct board_tile board[SIZE][SIZE]) {
    int row, start_col, end_col;

    scanf(" %d %d %d", &row, &start_col, &end_col);

    if (row >= SIZE || row <= 0) return;

    if (check_for_turtle_on_row(board, row)) return;

    if (start_col >= SIZE) return;

    if (start_col < 0) start_col = 0;

    if (end_col >= SIZE) end_col = SIZE - 1;

    for (int col = start_col; col <= end_col; col++) {
        board[row][col].type = LOG;
    }
}

// clears a row. Cannot be called upon Lillypad or Bank. 
void clear(struct board_tile board[SIZE][SIZE]) {
    int row;
    scanf(" %d", &row);
    
    if (!validate_input(row)) return;

    if (check_for_frogger(board, row)) return;

    for (int col = 0; col < SIZE; col++) {
        enum tile_type spot_type = board[row][col].type;
        if (spot_type != LILLYPAD && spot_type != BANK) {
            board[row][col].type = WATER;
            board[row][col].has_bug = FALSE;
        }
    }
}

// Delete a log command. It finds a specific log, checks for its beginning log,
// then starts deleting from left to right. 
void delete_log(struct board_tile board[SIZE][SIZE]) {
    int row, col;

    scanf(" %d %d", &row, &col);

    if (!(validate_input(row) && validate_input(col))) return;

    if (board[row][col].type != LOG) return;

    int log_start_col = find_log_start_col(board, row, col);

    if (check_for_frogger_on_log(board, row, log_start_col)) return;

    while (log_start_col < SIZE && board[row][log_start_col].type == LOG) {
        board[row][log_start_col].type = WATER;
        log_start_col++;
    }

}

// Move frogger command. Takes input of direction, which is a char of 'w', 'a', 's' or 'd'. 
void move_frogger(struct board_tile board[SIZE][SIZE], char direction, struct frogger_data *frogger) {
    move_bugs(board);

    int frog_row = frogger->row;
    int frog_col = frogger->col;

    int valid_move;

    switch (direction) {
        case 'w':
            if (frog_row == 0) return;

            valid_move = execute_move(board, frogger, -1, 0);
            if (valid_move) frogger->row--;
            break;

        case 'a':
            if (frog_col == 0) return;

            valid_move = execute_move(board, frogger, 0, -1);
            if (valid_move) frogger->col--;
            break;

        case 'd':
            if (frog_col ==  SIZE - 1) return;

            valid_move = execute_move(board, frogger, 0, 1);
            if (valid_move) frogger->col++;
            break;

        case 's':
            if (frog_row == SIZE - 1) return;

            valid_move = execute_move(board, frogger, 1, 0);
            if (valid_move) frogger->row++; 
            break;
        default:
            break;
    }

    check_game_win(board, frogger);
}

// ADD bug command. Takes an input of row and col, and will only add bugs to 
// rows and cols that have validated input and are only on spaces of type log or turtle.
void add_bug(struct board_tile board[SIZE][SIZE]) {
    int row, col;

    scanf(" %d %d", &row, &col);

    if (!(validate_input(row) && validate_input(col))) return;

    if (board[row][col].occupied || board[row][col].has_bug) return;

    if (board[row][col].type != TURTLE && board[row][col].type != LOG) return;

    board[row][col].has_bug = TRUE;
    board[row][col].bug_dir = RIGHT;
    board[row][col].bug_moved = FALSE;
}


/////////////////////////////// UTILITY FUNCTIONS //////////////////////////////

// sets up the board.
void setup_board(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger) {

    // sets up the first row with lilypads
    for (int i = 0; i < SIZE; i += 2) {
        struct board_tile lilly_pad = {
            .type = LILLYPAD,
            .occupied = FALSE,
            .has_bug = FALSE,
            .bug_dir = NONE,
        };
        board[0][i] = lilly_pad;
    } 
    
    // fill the rest of the first row with water.
    for (int i = 1; i < SIZE; i += 2) {
        board[0][i].type = WATER;
        board[0][i].occupied = FALSE;
        board[0][i].has_bug = FALSE;
        board[0][i].bug_dir = NONE;
    }

    // fill the middle rows and cols with water.

    for (int row = 1; row < SIZE - 1; row++) {
        for (int col = 0; col < SIZE; col++) {
            board[row][col].type = WATER;
            board[row][col].occupied = FALSE;
            board[row][col].has_bug = FALSE;
            board[row][col].bug_dir = NONE;
        }
    }

    // fill the final with water and put the middle with a frog.

    for (int col = 0; col < SIZE; col++) {
        board[SIZE - 1][col].type = BANK;
        board[SIZE - 1][col].has_bug = FALSE;
        board[SIZE - 1][col].bug_dir = NONE;
        if (col == SIZE / 2) {
            board[SIZE - 1][col].occupied = TRUE;

            frogger->row = SIZE - 1;
            frogger->col = col;
            frogger->lives = MAX_LIVES;
            frogger->win = FALSE;
        } else {
            board[SIZE - 1][col].occupied = FALSE;
        }
    } 

}

// adds turtles to the board.
void add_turtles(struct board_tile board[SIZE][SIZE], int turtle_num) {

    if (turtle_num <= 0) return;

    printf("Enter pairs: \n");
    int row;
    int col;

    for (int i = 0; i < turtle_num; i++) {
        scanf(" %d %d", &row, &col);

        if ((row >= 0 && row < SIZE) && (col >= 0 && col < SIZE)) {
            board[row][col].type = TURTLE;
        } 
    }
}

// handles the commands and calls the command functions when a specific command is recognised.
void command_handler(struct board_tile board[SIZE][SIZE], char command, struct frogger_data *frogger) {
    switch (command) {
        case 'l':
            add_log(board);
            break;
        case 'c':
            clear(board);
            break;
        case 'r':
            delete_log(board);
            break;
        case 'w':
            move_frogger(board, command, frogger);
            break;
        case 'a':
            move_frogger(board, command, frogger);
            break;
        case 's':
            move_frogger(board, command, frogger);
            break;
        case 'd':
            move_frogger(board, command, frogger);
            break;
        case 'b':
            add_bug(board);
            break;
        default:
            break;
    }
}

// checks for a turtle in a row. Returns TRUE or FALSE.
int check_for_turtle_on_row(struct board_tile board[SIZE][SIZE], int row) {
    for (int col = 0; col < SIZE; col++) {
        if (board[row][col].type == TURTLE) return TRUE;
    }

    return FALSE;
}

// checks for frogger in a row. Returns TRUE or FALSE.
int check_for_frogger(struct board_tile board[SIZE][SIZE], int row) {
    for (int col = 0; col < SIZE; col++) {
        if (board[row][col].occupied) return TRUE;
    }

    return FALSE;
}

// checks for frogger on a specific log. It will return TRUE or FALSE.
int check_for_frogger_on_log(struct board_tile board[SIZE][SIZE], int row, int log_start) {
    int i = log_start;
    while (i < SIZE && board[row][i].type == LOG) {
        if (board[row][i].occupied) return TRUE; 
        i++;
    }

    return FALSE;
}

// finds the beginning col number of a log on a certain row. It loops through the logs left of the given log till it 
// either finds water or the edge of the board. It returns the column value of the log start.
int find_log_start_col(struct board_tile board[SIZE][SIZE], int row, int col) {
    while (col > 0) {
        if (board[row][col - 1].type == WATER) return col;
        
        col--;
    }

    return col;
}

// checks if row or col inputs are valid. returns TRUE or FALSE.
int validate_input(int row_or_col) {
    return (row_or_col >= 0 && row_or_col < SIZE);
}

// called every time a move is made to see if frogger has made it to the other side.
void check_game_win(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger) {
    if (board[frogger->row][frogger->col].type == LILLYPAD) {
        frogger->win = TRUE;
        return;
    }
}

// prints the board and the live status whenever frogger encouters a Bug or a Water.
void print_lives_status(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger) {
    print_board(board);

    if (frogger->lives == 0) {
        printf("\n !! GAME OVER !! \n\n");
        return;
    }
    printf("\n# LIVES LEFT: %d #\n\n", frogger->lives);
}

// actually does the move execution. It returns TRUE if a valid move is made, i.e. frogger doesn't encounter bugs or water on their
// move. 
// It takes inputs of row_mod and col_mod, which essentially indicates the direction of the move.
// E.g. a move 'up' or 'w', would be row_mod = -1, and col_mod = 0, since we need to move one row up. 
int execute_move(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger, int row_mod, int col_mod) {
    int frog_row = frogger->row;
    int frog_col = frogger->col;

    board[frog_row + row_mod][frog_col + col_mod].occupied = TRUE;
    board[frog_row][frog_col].occupied = FALSE;

    if (board[frog_row + row_mod][frog_col + col_mod].type == WATER || board[frog_row + row_mod][frog_col + col_mod].has_bug) {
        frogger->lives--;
        print_lives_status(board, frogger);
        board[frog_row + row_mod][frog_col + col_mod].occupied = FALSE;
        return_frogger_start(board, frogger);
        return FALSE;
    }

    return TRUE;

}


// Called to return frogger to the start of the board and changes its data to fit it accordingly.
void return_frogger_start(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger) {
    board[SIZE - 1][SIZE / 2].occupied = TRUE;
    frogger->row = SIZE - 1;
    frogger->col = SIZE / 2;
}

void move_bugs(struct board_tile board[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (board[row][col].has_bug && !board[row][col].bug_moved) {
                /*
                for (int j = 0; j < SIZE; j++) {
                    printf("row: %d, col: %d HAS BUG: %d\n", row, j, board[row][j].has_bug);
                    //printf("HAS BUG: %d\n", board[row][j].has_bug);
                }*/
                move_single_bug(board, row, col);
            }
        }
    }

    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (board[row][col].has_bug) {
                board[row][col].bug_moved = FALSE;
            }
        }
    }
}

void move_single_bug(struct board_tile board[SIZE][SIZE], int row, int col) {
    enum bug_move_dir bug_dir = board[row][col].bug_dir;

    enum available_space valid_spaces = check_bug_space(board, row, col);

    if (valid_spaces == BOTH_SPACES) {
        if (bug_dir == RIGHT) {
            execute_bug_move(board, row, col, 1);
        } else {
            execute_bug_move(board, row, col, -1);
        }

    } else if (valid_spaces == RIGHT_SPACE) {
        execute_bug_move(board, row, col, 1);

    } else if (valid_spaces == LEFT_SPACE) {
        execute_bug_move(board, row, col, -1);
    }

}

enum available_space check_bug_space(struct board_tile board[SIZE][SIZE], int row, int col) {
    if (col == 0) {
        if (!is_available_for_bug(board, row, col, 1)) return NO_SPACE;
        return RIGHT_SPACE;
    } else if (col == SIZE - 1) {
        if (!is_available_for_bug(board, row, col, -1)) return NO_SPACE;

        return LEFT_SPACE;
    }


    int right_available = is_available_for_bug(board, row, col, 1);
    int left_available = is_available_for_bug(board, row, col, -1);

    if (left_available && right_available) { 
        //printf("BOTH!\n"); 
        return BOTH_SPACES;
        
    }
    if (right_available) {
        //printf("RIGHT!\n");
        return RIGHT_SPACE;
    }
    if (left_available) { 
        //printf("LEFT!\n");
        return LEFT_SPACE;
    }
    
    return NO_SPACE;
}


int is_available_for_bug(struct board_tile board[SIZE][SIZE], int row, int col, int col_mod) {
    struct board_tile check_tile = board[row][col + col_mod];

    if ((check_tile.type != LOG && check_tile.type != TURTLE) || check_tile.has_bug) return FALSE;

    return TRUE;
}

void execute_bug_move(struct board_tile board[SIZE][SIZE], int row, int col, int col_mod) {
    board[row][col].has_bug = FALSE;
    board[row][col].bug_dir = NONE;

    board[row][col + col_mod].has_bug = TRUE;
    board[row][col + col_mod].bug_dir = col_mod == 1 ? RIGHT : LEFT;
    board[row][col + col_mod].bug_moved = TRUE;

}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////// PROVIDED FUNCTIONS //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void print_board(struct board_tile board[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            char type_char = '\0';
            if (board[row][col].occupied) {
                type_char = 'F';
            } else if (board[row][col].has_bug) {
                type_char = 'B';
            } else {
                type_char = type_to_char(board[row][col].type);
            }
            printf("%c ", type_char);
        }
        printf("\n");
    }
}

char type_to_char(enum tile_type type) {
    char type_char = ' ';
    if (type == LILLYPAD) {
        type_char = 'o';
    } else if (type == BANK) {
        type_char = 'x';
    } else if (type == WATER) {
        type_char = '~';
    } else if (type == TURTLE) {
        type_char = 'T';
    } else if (type == LOG) {
        type_char = 'L';
    }
    return type_char;
}
