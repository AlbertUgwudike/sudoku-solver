#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cassert>
#include "sudoku.h"

using namespace std;

int global_counter = 0;

bool is_valid_entry(char c) { return c >= '1' && c <= '9'; }

bool cross_sqr_complete(char board[9][9], int idx, bool ignore_non_numeric) {
  
  for (int mode = 0; mode < 3; mode++) {
    
    bool encountered[9] = { false };

    for (int i = 0; i < 9; i++) {
      global_counter++;
      
      char entry_arr[3] = {
        board[idx / 9][i],
        board[i][idx % 9],
        board[3 * (idx / 27) + (i / 3)][3 * ((idx % 9) / 3) + (i % 3)]
      };
      char entry = entry_arr[mode];
      bool entry_valid = is_valid_entry(entry);

      if (!entry_valid && ignore_non_numeric) continue;
      if (!entry_valid) return false;
      if (encountered[entry - '1']) return false;
      
      encountered[entry - '1'] = true;
    }
  }
  
  return true;
}
  
// determine if board is fully completed
bool is_complete(char board[9][9]) {
  for (int i = 0; i < 9; i++) {
    // check if the i-th row and col are complete
    if (!cross_sqr_complete(board, i * 9 + i, false)) return false;

    // check if i-th sqr is complete
    if (!cross_sqr_complete(board, 27 * (i / 3) + 3 * (i % 3), false)) return false;
  }

  return true;
}

// check that entry at given position leaves board in valid state
bool check_position(char board[9][9], int idx) {
  int row_idx = idx / 9;
  int col_idx = idx % 9;
  int sqr_idx = 3 * (row_idx / 3) + (col_idx / 3);
  return cross_sqr_complete(board, 9 * row_idx + col_idx, true);
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
  if (!is_valid_entry(entry)) {
    cout << "\n\n" << "make_move - ERROR: 'entry' must be a number between 1 and 9 inclusive\n" << endl;
    return false;
  }

  int row_idx = pos[0] - 'A';
  int col_idx = pos[1] - '1';
  int idx = 9 * row_idx + col_idx;
  char tmp = board[row_idx][col_idx];
  board[row_idx][col_idx] = entry;

  // if the change cuased the board to become invalid reverse it
  if (!check_position(board, idx)) {
    board[row_idx][col_idx] = tmp;
    return false;
  }
  
  return true;
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

void initialise_possibility_board(char board[9][9], bool possibility_board[9][9][10], bool next) {
  for (int idx = 0; idx < 81; idx++) {
    int row_idx = idx / 9;
    int col_idx = idx % 9;

    // given cells are indicated in last layer
    if (is_valid_entry(board[row_idx][col_idx])) {
      possibility_board[row_idx][col_idx][9] = next ? possibility_board[row_idx][col_idx][9] : true;
      continue;
    }

    for (int possible_idx = 0; possible_idx < 9; possible_idx++) {
      board[row_idx][col_idx] = '1' + possible_idx;
      if (check_position(board, idx)) {
        possibility_board[row_idx][col_idx][possible_idx] = true;
      }
      board[row_idx][col_idx] = '.';
    }
  }

  global_counter += 729;
}

// determine number of possible options for a given cell
int count_possible(bool possibility_board[9][9][10], int idx) {
  int count = 0;
  for (int i = 0; i < 9; i++) {
    if (possibility_board[idx / 9][idx % 9][i]) count += 1;
  }
  
  global_counter += 9;
  return count;
}

// returns linearised idx of the empty cell with fewest possible options
// returns -1 if all cells have zero options remaining
int cell_with_fewest_options(char board[9][9], bool possibility_board[9][9][10]) {
  int least_options = 10;
  int idx_with_fewest_options = -1;
  
  for (int idx = 0; idx < 81; idx++) {
    global_counter += 1;
    char entry = board[idx / 9][idx % 9];

    // ignore already filled i cells
    if (is_valid_entry(entry)) continue;
    
    int number_of_options = count_possible(possibility_board, idx);

    // no need to continue if 1 option (minmum) or 0 options (contradiction)
    // contradition state is dealt with in r_solve_board (loop exits)
    if (number_of_options <= 1) return idx;
    
    if (number_of_options < least_options) {
      least_options = number_of_options;
      idx_with_fewest_options = idx;
    }
  }

  return idx_with_fewest_options;
}

void update_possibility_board(bool possibility_board[9][9][10], int idx, int poss_idx, bool val) {
  int row_idx = idx / 9;
  int col_idx = idx % 9;
  
  // set the possibility to false in the cell in questoin
  possibility_board[row_idx][col_idx][poss_idx] = val;

  // set to false in cells dependent on this cell
  for (int i = 0; i < 9; i++) {
    // ignore 'givem' cells;
    if (possibility_board[i][col_idx][9]) continue;
    possibility_board[i][col_idx][poss_idx] = val;
  }

  for (int i = 0; i < 9; i++) {
    // ignore 'given' cells
    if (possibility_board[row_idx][i][9]) continue;
    possibility_board[row_idx][i][poss_idx] = val;
  }
  
  int sqr_row = 3 * (row_idx / 3);
  int sqr_col = 3 * (col_idx / 3);
  for (int i = sqr_row; i < sqr_row + 3; i++)
    for (int j = sqr_col; j < sqr_col + 3; j++) {
      // ignore given cells
      if (possibility_board[i][j][9]) continue;
      possibility_board[i][j][poss_idx] = val;
    }

  global_counter += 27;
}

void take_snapshot(bool possibility_board[9][9][10], bool snapshot[3][9], int idx, int poss_idx) {
  int row_idx = idx / 9;
  int col_idx = idx % 9;

  for (int i = 0; i < 9; i++) {
    snapshot[0][i] = possibility_board[row_idx][i][poss_idx];
    snapshot[1][i] = possibility_board[i][col_idx][poss_idx];
  }

  int sqr_row = 3 * (row_idx / 3);
  int sqr_col = 3 * (col_idx / 3);
  for (int i = sqr_row; i < sqr_row + 3; i++)
    for(int j = sqr_col; j < sqr_col + 3; j++) {
      snapshot[2][(i - sqr_row) * 3 + j - sqr_col] = possibility_board[i][j][poss_idx];
    }

  global_counter += 18;
}
  
void reset_from_snapshot(bool possibility_board[9][9][10], bool snapshot[3][9], int idx, int poss_idx) {
  int row_idx = idx / 9;
  int col_idx = idx % 9;

  for (int i = 0; i < 9; i++) {
    possibility_board[row_idx][i][poss_idx] = snapshot[0][i];
    possibility_board[i][col_idx][poss_idx] = snapshot[1][i];
  }

  int sqr_row = 3 * (row_idx / 3);
  int sqr_col = 3 * (col_idx / 3);
  for (int i = sqr_row; i < sqr_row + 3; i++)
    for(int j = sqr_col; j < sqr_col + 3; j++) {
      possibility_board[i][j][poss_idx] = snapshot[2][(i - sqr_row) * 3 + j - sqr_col];
    }
  
  global_counter += 18;
}

bool r_solve_board(char board[9][9], bool possibility_board[9][9][10]) {
  
  int idx = cell_with_fewest_options(board, possibility_board);
  if (idx == -1) return true;
  
  int row_idx = idx / 9;
  int col_idx = idx % 9;

  for (int poss_idx = 0; poss_idx < 9; poss_idx++) {
    // ignore entries that are not permitted
    if (!possibility_board[row_idx][col_idx][poss_idx]) continue;

    // insert character represented by possibility into board
    board[row_idx][col_idx] = '1' + poss_idx;

    // take 'snapshot' of relevant cells in case we ned to reset later
    // snapshot[0] == row, snapshot[1] == col, snapshot[2] = sqr
    bool snapshot[3][9] = { false };
    take_snapshot(possibility_board, snapshot, idx, poss_idx);
    
    // update possibililty board
    update_possibility_board(possibility_board, idx, poss_idx, false);

    // contine recursion
    bool solved = r_solve_board(board, possibility_board);

    if (solved) return true;

    // erase board at possition
    board[row_idx][col_idx] = '.';

    // restore the possibility board;
    reset_from_snapshot(possibility_board, snapshot, idx, poss_idx);
  }

  // loop compoletes no valid state found
  return false;
}

bool solve_board(char board[9][9]) {
  bool possibility_board[9][9][10] = { false };
  initialise_possibility_board(board, possibility_board, false);
  global_counter = 0;
  bool solved =  r_solve_board(board, possibility_board);
  cout << "COUNTER:" << global_counter << endl;
  global_counter = 0;
  return solved;
}
    
void solve_board_file(char *filename) {
  char board[9][9] = { '.' };
  load_board(filename, board);
  if (solve_board(board)) {
    cout << filename << " has a solution:\n";
    display_board(board);
  } else {
    cout << "A solution could not be found.\n" << endl;
  }
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
void display_board(char board[9][9]) {
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
