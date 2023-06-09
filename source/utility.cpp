#include "../headers/utility.h"

#include "../headers/include.h"

void load_board(const char* filename, char board[9][9]) {
    cout << "Loading Sudoku board from file '" << filename << "'... ";

    ifstream in(filename);
    if (!in) {
        cout << "Failed!\n";
    }
    assert(in);

    char buffer[512];

    int row = 0;
    in.getline(buffer, 512);
    while (in && row < 9) {
        for (int n = 0; n < 9; n++) {
            assert(buffer[n] == '.' || isdigit(buffer[n]));
            board[row][n] = buffer[n];
        }
        row++;
        in.getline(buffer, 512);
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
    cout << (char)('A' + row) << " ";
    for (int i = 0; i < 9; i++) {
        cout << ((i % 3) ? ':' : '|') << " ";
        cout << ((data[i] == '.') ? ' ' : data[i]) << " ";
    }
    cout << "|\n";
}

/* pre-supplied function to display a Sudoku board */
void display_board(char board[9][9]) {
    cout << "    ";
    for (int r = 0; r < 9; r++) {
        cout << (char)('1' + r) << "   ";
    }
    cout << '\n';
    for (int r = 0; r < 9; r++) {
        print_frame(r);
        print_row(board[r], r);
    }
    print_frame(9);
}

/* add your functions here */