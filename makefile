sudoku: main.o sudoku.o utility.o
	g++ main.o sudoku.o utility.o -o sudoku
	make clean

main.o: source/main.cpp headers/sudoku.h
	g++ -c source/main.cpp

utility.o: source/utility.cpp
	g++ -c -g source/utility.cpp

sudoku.o: source/sudoku.cpp
	g++ -c -g source/sudoku.cpp

clear:
	$(RM) *.o sudoku

clean:
	$(RM) *.o
