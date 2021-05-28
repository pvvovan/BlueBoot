all: move

CPP_SOURCES = \
hwioab.cpp \
control_helper.cpp

C_SOURCES = \
move.c

OBJECTS = $(C_SOURCES:.c=.o)
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS += $(CPP_SOURCES:.cpp=.o)
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

%.o: %.c %.h Makefile
	clang -std=gnu18 -Wall -Wextra -Wpedantic $< -c -O3

%.o: %.cpp %.hpp Makefile
	clang++ -std=c++17 -Wall -Wextra -Wpedantic $< -c -O3

move: $(OBJECTS) Makefile
	clang $(OBJECTS) -o $@ -lstdc++ -lpthread -latomic -O3

.PHONY: clean
clean:
	rm move $(OBJECTS)
