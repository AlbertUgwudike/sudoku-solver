#include "../headers/include.h"
#include "../headers/sudoku.h"
#include "../headers/utility.h"

using namespace std;

int main() {
    solve_board_file("boards/easy.dat");
    solve_board_file("boards/medium.dat");
    solve_board_file("boards/mystery1.dat");
    solve_board_file("boards/mystery2.dat");
    solve_board_file("boards/mystery3.dat");
    solve_board_file("boards/empty.dat");
    solve_board_file("boards/diagonal.dat");
    solve_board_file("boards/singular.dat");
    solve_board_file("boards/contradiction.dat");

    return 0;
}
