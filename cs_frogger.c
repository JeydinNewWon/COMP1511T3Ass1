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

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  STRUCTS  //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Provided structs
struct board_tile {
    enum tile_type type; // The type of piece it is (water, bank, etc.)
    int occupied;        // TRUE or FALSE based on if Frogger is there.
    enum tile_type prev_type;
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
int check_for_turtle(struct board_tile board[SIZE][SIZE], int row);
int validate_input(int row_or_col);
int check_for_frogger(struct board_tile board[SIZE][SIZE], int row);
int check_for_frogger_on_log(struct board_tile board[SIZE][SIZE], int row, int log_start_col);
int find_log_start_col(struct board_tile board[SIZE][SIZE], int row, int col);
void print_lives_status(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger);
void check_game_win(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger);
int execute_move(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger, int row_mod, int col_mod);
void return_frogger_start(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger);



// Commands
void add_log(struct board_tile board[SIZE][SIZE]);
void clear(struct board_tile board[SIZE][SIZE]);
void delete_log(struct board_tile board[SIZE][SIZE]);
void move_frogger(struct board_tile board[SIZE][SIZE], char direction, struct frogger_data *frogger);


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


void add_log(struct board_tile board[SIZE][SIZE]) {
    int row, start_col, end_col;

    scanf(" %d %d %d", &row, &start_col, &end_col);

    if (row >= SIZE || row <= 0) return;

    if (check_for_turtle(board, row)) return;

    if (start_col >= SIZE) return;

    if (start_col < 0) start_col = 0;

    if (end_col >= SIZE) end_col = SIZE - 1;

    for (int col = start_col; col <= end_col; col++) {
        board[row][col].type = LOG;
    }
}

void clear(struct board_tile board[SIZE][SIZE]) {
    int row;
    scanf(" %d", &row);
    
    if (!validate_input(row)) return;

    if (check_for_frogger(board, row)) return;

    for (int col = 0; col < SIZE; col++) {
        enum tile_type spot_type = board[row][col].type;
        if (spot_type != LILLYPAD && spot_type != BANK) {
            board[row][col].type = WATER;
        }
    }
}

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


    // checks for log bits on left

}

void move_frogger(struct board_tile board[SIZE][SIZE], char direction, struct frogger_data *frogger) {
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


/////////////////////////////// UTILITY FUNCTIONS //////////////////////////////

void setup_board(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger) {

    // sets up the first row with lilypads
    for (int i = 0; i < SIZE; i += 2) {
        struct board_tile lilly_pad = {
            .type = LILLYPAD,
            .occupied = FALSE,
        };
        board[0][i] = lilly_pad;
    } 
    
    // fill the rest of the first row with water.
    for (int i = 1; i < SIZE; i += 2) {
        board[0][i].type = WATER;
        board[0][i].occupied = FALSE;
    }

    // fill the middle rows and cols with water.

    for (int row = 1; row < SIZE - 1; row++) {
        for (int col = 0; col < SIZE; col++) {
            board[row][col].type = WATER;
            board[row][col].occupied = FALSE;
        }
    }

    // fill the final with water and put the middle with a frog.

    for (int col = 0; col < SIZE; col++) {
        board[SIZE - 1][col].type = BANK;
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
        default:
            break;
    }
}

int check_for_turtle(struct board_tile board[SIZE][SIZE], int row) {
    for (int col = 0; col < SIZE; col++) {
        if (board[row][col].type == TURTLE) return TRUE;
    }

    return FALSE;
}

int check_for_frogger(struct board_tile board[SIZE][SIZE], int row) {
    for (int col = 0; col < SIZE; col++) {
        if (board[row][col].occupied) return TRUE;
    }

    return FALSE;
}

int check_for_frogger_on_log(struct board_tile board[SIZE][SIZE], int row, int log_start) {
    int i = log_start;
    while (i < SIZE && board[row][i].type == LOG) {
        if (board[row][i].occupied) return TRUE; 
        i++;
    }

    return FALSE;
}

int find_log_start_col(struct board_tile board[SIZE][SIZE], int row, int col) {
    while (col > 0) {
        if (board[row][col - 1].type == WATER) return col;
        
        col--;
    }

    return col;
}

int validate_input(int row_or_col) {
    return (row_or_col >= 0 && row_or_col < SIZE);
}

void check_game_win(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger) {
    if (board[frogger->row][frogger->col].type == LILLYPAD) {
        frogger->win = TRUE;
        return;
    }
}

void print_lives_status(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger) {
    print_board(board);

    if (frogger->lives == 0) {
        printf("\n !! GAME OVER !! \n\n");
        return;
    }
    printf("\n# LIVES LEFT: %d #\n\n", frogger->lives);
}

int execute_move(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger, int row_mod, int col_mod) {
    int frog_row = frogger->row;
    int frog_col = frogger->col;

    board[frog_row + row_mod][frog_col + col_mod].occupied = TRUE;
    board[frog_row][frog_col].occupied = FALSE;

    if (board[frog_row + row_mod][frog_col + col_mod].type == WATER) {
        frogger->lives--;
        print_lives_status(board, frogger);
        board[frog_row + row_mod][frog_col + col_mod].occupied = FALSE;
        return_frogger_start(board, frogger);
        return FALSE;
    }

    return TRUE;

}

void return_frogger_start(struct board_tile board[SIZE][SIZE], struct frogger_data *frogger) {
    board[SIZE - 1][SIZE / 2].occupied = TRUE;
    frogger->row = SIZE - 1;
    frogger->col = SIZE / 2;
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
