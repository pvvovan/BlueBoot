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
	gcc -std=gnu18 -Wall -Wextra -Wpedantic $< -c

%.o: %.cpp %.hpp Makefile
	g++ -std=c++17 -Wall -Wextra -Wpedantic $< -c

move: $(OBJECTS) Makefile
	gcc $(OBJECTS) -o $@ -lstdc++ -lpthread -latomic

.PHONY: clean
clean:
	rm move $(OBJECTS)
