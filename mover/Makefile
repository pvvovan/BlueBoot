all: move

move: move.c hwioab.cpp control_helper.cpp
	g++ -std=c++17 -Wall -Wextra -Wpedantic control_helper.cpp -c
	g++ -std=c++17 -Wall -Wextra -Wpedantic hwioab.cpp -c
	gcc -std=gnu18 -Wall -Wextra -Wpedantic move.c -c
	gcc move.o hwioab.o control_helper.o -o move -lstdc++ -lpthread -latomic

.PHONY: clean
clean:
	rm move move.o hwioab.o control_helper.o
