
SOURCE  := $(wildcard src/*.c src/*/*.c)
HEADER  := $(wildcard src/*.h src/*/*.h)
OBJECTS := $(SOURCE:src/%.c=bin/%.o)

EXEC := out/helium
TEST_FLAGS := test.he

CC := gcc
CC_FLAGS := -g -c -Wall -Wno-unused-variable
DB := gdb
DB_FLAGS := $(EXEC)


all: $(EXEC)


test: $(EXEC)
	$(EXEC) demo/test.he


bin/%.o: src/%.c
	$(CC) $(CC_FLAGS) $< -o $@


$(EXEC): $(OBJECTS)
	$(CC) -g $^ -o $@


clean:
	rm $(OBJECTS) $(EXEC)


debug:
	$(DB) $(DB_FLAGS)
