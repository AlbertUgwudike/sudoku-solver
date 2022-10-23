#ifndef SUDOKU_H
#define SUDOKU_H

enum Direction { COL, ROW };
enum On_non_numeric { NON_NUMERIC_INVALID, IGNORE_NON_NUMERIC };

void load_board(const char* filename, char board[9][9]);
void display_board(const char board[9][9]);
bool row_or_col_complete(char board[9][9], int idx, Direction direction);
bool col_completed(char board[9][9], int col_idx);
bool row_completed(char board[9][9], int row_idx);
bool sqr_completed(char board[9][9], int sqr_idx);
bool is_complete(char board[9][9]);
bool make_move(const char pos[2], char entry, char board[9][9]);
bool check_position(char board[9][9], int row_idx, int col_idx);
bool save_board(const char *filename, char board[9][9]);
bool solve_board(char board[9][9]);

#endif
