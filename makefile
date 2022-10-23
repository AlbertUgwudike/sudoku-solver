sudoku: main.o sudoku.o
	g++ main.o sudoku.o -o sudoku

main.o: main.cpp sudoku.h
	g++ -c main.cpp

sudoku.o:
	g++ -c sudoku.cpp

clean:
	$(RM) *.o sudoku
