
CXX = g++
CXXFLAGS = -O0 -g3 -std=c++17

main: main.cpp TextGame.o
	$(CXX) $(CXXFLAGS) main.cpp TextGame.o -o main

TextGame.o: TextGame.cpp TextGame.h
	$(CXX) $(CXXFLAGS) -c TextGame.cpp -o TextGame.o

deepclean:
	rm -f *~ *.o main main.exe *.stackdump

clean:
	rm -f *~ *.o *.stackdump