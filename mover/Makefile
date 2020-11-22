all: move

move: move.c hwioab.cpp
	g++ -std=c++17 -Wall -Wextra -Wpedantic hwioab.cpp -c
	gcc -std=gnu18 -Wall -Wextra -Wpedantic move.c -c
	gcc move.o hwioab.o -o move -lstdc++ -lpthread

.PHONY: clean
clean:
	rm move move.o hwioab.o
