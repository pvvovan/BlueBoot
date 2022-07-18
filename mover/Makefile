all: move

CPP_SOURCES = \
hwioab.cpp \
control_helper.cpp

C_SOURCES = \
move.c \
move_cmd.c

CC_FLAGS = -O3 -Wall -Werror -Wextra -pedantic -pedantic-errors -Wconversion -Wsign-conversion -Werror=pedantic

OBJECTS = $(C_SOURCES:.c=.o)
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS += $(CPP_SOURCES:.cpp=.o)
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

%.o: %.c *.h Makefile
	$(CC) $(CC_FLAGS) -std=gnu18 $< -c

%.o: %.cpp *.hpp *.h Makefile
	$(CC) $(CC_FLAGS) -std=c++17 $< -c

move: $(C_SOURCES:.c=.o) $(CPP_SOURCES:.cpp=.o) Makefile
	$(CC) $(OBJECTS) -o $@ -lstdc++ -lpthread -latomic -O3

.PHONY: clean
clean:
	-rm -f move $(OBJECTS)
