#ifndef SUDOKU_H
#define SUDOKU_H

void load_board(const char* filename, char board[9][9]);
void display_board(char board[9][9]);
bool is_complete(char board[9][9]);
bool make_move(const char pos[2], char entry, char board[9][9]);
bool save_board(const char *filename, char board[9][9]);
bool solve_board(char board[9][9]);
void solve_board_file(const char *filename);

#endif
