#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cassert>
#include "sudoku.h"

using namespace std;

int count = 0;

bool row_or_col_complete(char board[9][9], int idx, Direction dir, On_non_numeric onn) {
  // keep track of already counted numbers
  bool present[9] = { false };

  for (int i = 0; i < 9; i++) {
    count += 1;
    char entry = (dir == ROW ? board[idx][i] : board[i][idx]);
    if (entry < '1' || entry > '9') return onn == IGNORE_NON_NUMERIC;
    if (present[entry - '1']) return false;
    else present[entry - '1'] = true;
  }

  return true;
}

bool col_completed(char board[9][9], int col_idx, On_non_numeric onn) {
  return row_or_col_complete(board, col_idx, COL, onn);
}

bool row_completed(char board[9][9], int row_idx, On_non_numeric onn) {
  return row_or_col_complete(board, row_idx, ROW, onn);
}

bool sqr_completed(char board[9][9], int sqr_idx, On_non_numeric onn) {
  /*                                 
     sqr --> square.           012
     Each 3x3 sqr is indexed   345
     as shown on the right.    678
  */

  int row_idx = sqr_idx - (sqr_idx % 3);
  int col_idx = 3 * (sqr_idx % 3);
  bool present[9] = { false };

  for (int i = row_idx; i < row_idx + 3; i++)
    for(int j = col_idx; j < col_idx + 3; j++) {
      count += 1;
      char entry = board[i][j];
      if (entry < '1' || entry > '9') return onn == IGNORE_NON_NUMERIC;
      if (present[entry - '1']) return false;
      else present[entry - '1'] = true;
    }

  return true;
}
  
// O(n): determines if a given board is completed correctly
bool is_complete(char board[9][9]) {
  for (int i = 0; i < 9; i++) {
    // check if the i-th row, col and 3x3 square are complete
    bool complete = row_completed(board, i, NON_NUMERIC_INVALID) &&
                    col_completed(board, i, NON_NUMERIC_INVALID) &&
                    sqr_completed(board, i, NON_NUMERIC_INVALID);

    if (!complete) return false;
  }

  return true;
}

// attempt to insert entry intro board
// return false and revert board if invalid
bool make_move(const char pos[2], char entry, char board[9][9]) {
  // check position is in bounds
  if (pos[0] < 'A' || pos[1] > 'I' || pos[1] < '1' || pos[1] > '9') {
    cout << "\n\n" << "make_move - ERROR: 'pos' must take the form '<A-I><1-9>'\n" << endl;
    return false;
  }

  // check if entry is valid
  if (entry < '1' || entry > '9') {
    cout << "\n\n" << "make_move - ERROR: 'entry' must be a number between 1 and 9 inclusive\n" << endl;
    return false;
  }

  int row_idx = pos[0] - 'A';
  int col_idx = pos[1] - '1'; 
  char tmp = board[row_idx][col_idx];
  board[row_idx][col_idx] = entry;

  // if the change cuased the board to become invalid reverse it
  if (!check_position(board, row_idx, col_idx)) {
    board[row_idx][col_idx] = tmp;
    return false;
  }
  
  return true;
}

// check that entry at given position leaves board in valid state
bool check_position(char board[9][9], int row_idx, int col_idx) {
  int sqr_idx = (row_idx / 3) + (col_idx % 3);
  return row_completed(board, row_idx, IGNORE_NON_NUMERIC) &&
         col_completed(board, col_idx, IGNORE_NON_NUMERIC) &&
         sqr_completed(board, sqr_idx, IGNORE_NON_NUMERIC);
}

bool save_board(const char *filename, char board[9][9]) {
  ofstream out;
  out.open(filename);
  if (out.fail()) return false;
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      out.put(board[i][j]);
    }
    out.put('\n');
  }
  out.close();
  return true;
}

bool _solve_board(char board[9][9], int idx) {
  if (idx == 81) return true;

  int row_idx = idx / 9;
  int col_idx = idx % 9;

  // ignore given numbers
  if (board[row_idx][col_idx] != '.') return _solve_board(board, idx + 1);

  for (int i = 0; i < 9; i++) {
    board[row_idx][col_idx] = '1' + i;
    if (check_position(board, row_idx, col_idx) && _solve_board(board, idx + 1)) return true;
  }

  board[row_idx][col_idx] = '.';
  return false;
}

bool solve_board(char board[9][9]) {
  bool answer = _solve_board(board, 0);
  cout << "COUNT: " << count << endl;
  count = 0;
  return answer;
}


/* You are pre-supplied with the functions below. Add your own 
   function definitions to the end of this file. */

/* pre-supplied function to load a Sudoku board from a file */
void load_board(const char* filename, char board[9][9]) {

  cout << "Loading Sudoku board from file '" << filename << "'... ";

  ifstream in(filename);
  if (!in) {
    cout << "Failed!\n";
  }
  assert(in);

  char buffer[512];

  int row = 0;
  in.getline(buffer,512);
  while (in && row < 9) {
    for (int n=0; n<9; n++) {
      assert(buffer[n] == '.' || isdigit(buffer[n]));
      board[row][n] = buffer[n];
    }
    row++;
    in.getline(buffer,512);
  }

  cout << ((row == 9) ? "Success!" : "Failed!") << '\n';
  assert(row == 9);
}

/* internal helper function */
void print_frame(int row) {
  if (!(row % 3)) {
    cout << "  +===========+===========+===========+\n";
  } else {
    cout << "  +---+---+---+---+---+---+---+---+---+\n";
  }
}

/* internal helper function */
void print_row(const char* data, int row) {
  cout << (char) ('A' + row) << " ";
  for (int i=0; i<9; i++) {
    cout << ( (i % 3) ? ':' : '|' ) << " ";
    cout << ( (data[i]=='.') ? ' ' : data[i]) << " ";
  }
  cout << "|\n";
}

/* pre-supplied function to display a Sudoku board */
void display_board(const char board[9][9]) {
  cout << "    ";
  for (int r=0; r<9; r++) {
    cout << (char) ('1'+r) << "   ";
  }
  cout << '\n';
  for (int r=0; r<9; r++) {
    print_frame(r);
    print_row(board[r],r);
  }
  print_frame(9);
}

/* add your functions here */
