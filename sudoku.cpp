#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cassert>
#include "sudoku.h"

using namespace std;

bool is_valid_entry(char c) { return c >= '1' && c <= '9'; }

// determines if the row, col and minor-square associated with an index is completed
bool cross_sqr_complete(char board[9][9], int idx, bool ignore_non_numeric) {
  
  for (int mode = 0; mode < 3; mode++) {

    // keep track of entries already encountered
    bool encountered[9] = { false };

    for (int i = 0; i < 9; i++) {
      
      char entry_arr[3] = {
        board[idx / 9][i], /* row mode */
        board[i][idx % 9], /* col mode */

        /* minor square mode */
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
  // calls 'cross_sqr_complete' on the 9 main diagonal cells
  // and in one cell of each of the 9 minor (3x3) squares.
  // A few overlapping checks but overhead is minimal.
  
  for (int i = 0; i < 9; i++) {
    if (!cross_sqr_complete(board, i * 9 + i, false)) return false;
    if (!cross_sqr_complete(board, 27 * (i / 3) + 3 * (i % 3), false)) return false;
  }

  return true;
}

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
  if (!cross_sqr_complete(board, idx, true)) {
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

void initialise_possibility_board(char board[9][9], bool possibility_board[9][9][10]) {
  for (int idx = 0; idx < 81; idx++) {
    int row_idx = idx / 9;
    int col_idx = idx % 9;

    // clue cells are indicated in the 10th layer
    if (is_valid_entry(board[row_idx][col_idx])) {
      possibility_board[row_idx][col_idx][9] = true;
      continue;
    }

    // fill in possibilities
    for (int possible_idx = 0; possible_idx < 9; possible_idx++) {
      board[row_idx][col_idx] = '1' + possible_idx;
      if (cross_sqr_complete(board, idx, true)) {
        possibility_board[row_idx][col_idx][possible_idx] = true;
      }
      board[row_idx][col_idx] = '.';
    }
  }

}

// determine number of possible options for a given cell
int count_possible(bool possibility_board[9][9][10], int idx) {
  int count = 0;
  for (int i = 0; i < 9; i++) {
    if (possibility_board[idx / 9][idx % 9][i]) count += 1;
  }
  
  return count;
}

int cell_with_fewest_options(char board[9][9], bool possibility_board[9][9][10]) {
  int least_options = 10;
  int idx_with_fewest_options = -1;
  
  for (int idx = 0; idx < 81; idx++) {
    char entry = board[idx / 9][idx % 9];

    // ignore already filled-in cells
    if (is_valid_entry(entry)) continue;
    
    int number_of_options = count_possible(possibility_board, idx);

    // if there are zero options remaining at this exit, we've recahed a 'dead end'
    // if there is one option remaining, return early for efficiency
    if (number_of_options <= 1) return idx;
    
    if (number_of_options < least_options) {
      least_options = number_of_options;
      idx_with_fewest_options = idx;
    }
  }

  // if board is complete we return -1
  return idx_with_fewest_options;
}

void prune_possibilities(bool possibility_board[9][9][10], int idx, int poss_idx) {
  for (int mode = 0; mode < 3; mode++)
    for (int i = 0; i < 9; i++) {
      
      int idxs[3][2] = {
        { idx / 9, i }, /* row mode */
        { i, idx % 9 }, /* col mode */

        /* minor square mode */
        {
          3 * (idx / 27) + (i / 3),
          3 * ((idx % 9) / 3) + (i % 3)
        }
      };

      int row_idx = idxs[mode][0];
      int col_idx = idxs[mode][1];

      // ignore clue cells
      if (possibility_board[row_idx][col_idx][9]) continue;
      
      possibility_board[row_idx][col_idx][poss_idx] = false;
    }
}

// save the state (at 'poss_idx') of all cells dependent on cell at 'idx'.
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

}

// restore the state of th epossibility board using previously saved 'snapshot'
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
  
}

bool r_solve_board(char board[9][9], bool possibility_board[9][9][10], int &counter) {
  
  int idx = cell_with_fewest_options(board, possibility_board);

  // 'cell_with_fewest_options' returns -1 when all cells are filled in.
  if (idx == -1) return true;

  // continue despite discovering solution to reveal more
  // if (idx == -1) {
  //   display_board(board);
  //   return false;
  // }
  
  int row_idx = idx / 9;
  int col_idx = idx % 9;

  for (int poss_idx = 0; poss_idx < 9; poss_idx++) {
    // ignore entries that are not permitted
    if (!possibility_board[row_idx][col_idx][poss_idx]) continue;

    // insert character represented by possibility into board
    board[row_idx][col_idx] = '1' + poss_idx;

    // take 'snapshot' of relevant cells in case we need to backtrack.
    // snapshot[0] == row, snapshot[1] == col, snapshot[2] = sqr
    bool snapshot[3][9] = { false };
    take_snapshot(possibility_board, snapshot, idx, poss_idx);
    
    // update cells dependent on the cell we modified
    prune_possibilities(possibility_board, idx, poss_idx);

    // continue solving
    bool solved = r_solve_board(board, possibility_board, counter);

    if (solved) return true;

    // not solved, time to backtrack ...
    counter++;

    // restore the board 
    board[row_idx][col_idx] = '.';

    // restore the possibility board;
    reset_from_snapshot(possibility_board, snapshot, idx, poss_idx);
  }

  // loop completes, no valid combinations discovered
  return false;
}

bool solve_board(char board[9][9]) {
  
  // associates each cell with a boolean array whose elements indicate
  // whether the character represetnted by said element's idx ('1' + idx)
  // is a valid entry in that cell. The 10th boolean indicates clue cells
  // to be ignored. -----------------------------------------------------
  bool possibility_board[9][9][10] = { false };
  // --------------------------------------------------------------------

  initialise_possibility_board(board, possibility_board);
  
  int counter = 0; 
  bool solved =  r_solve_board(board, possibility_board, counter);
  
  cout << "\n" << counter << " backtracks attempted!\n" << endl;
  return solved;
}
    
void solve_board_file(const char *filename) {
  char board[9][9] = { '.' };
  cout << "----------------------------------" << endl;
  load_board(filename, board);
  if (solve_board(board)) {
    cout << filename << " has a solution:\n";
    display_board(board);
  } else {
    cout << "A solution could not be found.\n" << endl;
  }
  cout << "----------------------------------" << endl;
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
