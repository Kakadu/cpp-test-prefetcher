CXXFLAGS=-std=c++0x -O3

all: main

main: main.o
	$(CXX)  $< -o $@

plot:
	gnuplot test.plt


clean:
	rm -f main.o main
