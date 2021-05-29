all: jpgSrv

jpgSrv: jpgSrv.cpp
	g++ -O3 -Wall -Werror -Wextra -pedantic -pedantic-errors -Wconversion -Wsign-conversion -Werror=pedantic -std=c++17 jpgSrv.cpp -o jpgSrv -lstdc++fs

.PHONY: clean
clean:
	-rm -f jpgSrv
