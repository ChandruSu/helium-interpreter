.PHONY: test

SOURCE  := $(wildcard src/*.c src/*/*.c)
HEADER  := $(wildcard src/*.h src/*/*.h)
OBJECTS := $(SOURCE:src/%.c=bin/%.o)

EXEC := out/helium.exe
TEST_FLAGS := test/test.he

DEBUG :=
CC := gcc
CC_FLAGS := $(DEBUG) -c -Wall -Wno-unused-variable
DB := gdb
DB_FLAGS := -ex "lay src" -ex "break main" -ex "run $(TEST_FLAGS)"
PF := gprof
PF_FLAGS := $(EXEC) -f run_program

all: $(EXEC)


test: $(EXEC)
	$(EXEC) $(TEST_FLAGS)


bin/%.o: src/%.c
	$(CC) $(CC_FLAGS) $< -o $@


$(EXEC): $(OBJECTS)
	$(CC) $(DEBUG) $^ -o $@


clean:
	rm $(OBJECTS) $(EXEC)


debug: $(EXEC)
	$(DB) $(DB_FLAGS) $(EXEC)

profile:
	make test
	$(PF) $(PF_FLAGS)
