
SOURCE  := $(wildcard src/*.c src/*/*.c)
HEADER  := $(wildcard src/*.h src/*/*.h)
OBJECTS := $(SOURCE:src/%.c=out/%.o)

EXEC := out/output.exe
TEST_FLAGS := hi

CC := gcc
CC_FLAGS := -g -c -Wall

all: $(EXEC)
	./$(EXEC) $(TEST_FLAGS)

out/%.o: src/%.c
	$(CC) $(CC_FLAGS) $< -o $@

$(EXEC): $(OBJECTS)
	$(CC) $^ -o $@

clean:
	rm $(OBJECTS) $(EXEC)