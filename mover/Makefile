.DEFAULT_GOAL := move.elf

CPP_SOURCES = \
hwioab.cpp \
control_helper.cpp

C_SOURCES = \
move.c \
move_cmd.c

CC_FLAGS = -O0 -Og -g3 -Wall -Werror -Wextra -pedantic -pedantic-errors -Wconversion -Wsign-conversion -Werror=pedantic

OBJECTS = $(C_SOURCES:.c=.o)
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS += $(CPP_SOURCES:.cpp=.o)
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

%.o: %.c *.h Makefile
	gcc $(CC_FLAGS) -std=gnu18 $< -c

%.o: %.cpp *.hpp *.h Makefile
	g++ $(CC_FLAGS) -std=c++17 $< -c

move.elf: $(C_SOURCES:.c=.o) $(CPP_SOURCES:.cpp=.o) Makefile
	gcc $(OBJECTS) -o $@ -lstdc++ -lpthread -latomic ${CC_FLAGS}

.PHONY: clean
clean:
	-rm -f move.elf $(OBJECTS)
