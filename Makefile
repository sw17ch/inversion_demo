TEST_SRC=test/test.c
TEST_INC=test/greatest.h

PROG_INC=inc/bufferer.h
PROG_SRC=src/bufferer.c

PROG_INC_PATH=-Iinc/
TEST_INC_PATH=-Itest/

CFLAGS=-Wall -Werror -Wextra -O3 -g

default: compile

all: mod test demo

mod: build/empty_main

demo: build/demo
	./build/demo

test: build/test $(TEST_SRC) $(TEST_INC) $(PROG_INC) $(PROG_SRC)
	./build/test

compile: build/test build/empty_main

build/test: build $(TEST_SRC) $(TEST_INC) $(PROG_INC) $(PROG_SRC)
	$(CC) $(CFLAGS) $(PROG_INC_PATH) $(TEST_INC_PATH) $(TEST_SRC) $(PROG_SRC) -o build/test

build/empty_main: build test/empty_main.c $(PROG_SRC)
	$(CC) $(CFLAGS) $(PROG_INC_PATH) $(TEST_INC_PATH) $(PROG_SRC) test/empty_main.c -o build/empty_main
	./build/empty_main

build/demo: build demo/demo.c $(PROG_SRC)
	$(CC) $(CFLAGS) $(PROG_INC_PATH) $(PROG_SRC) demo/demo.c -o build/demo

# Create the build directory.
build:
	mkdir build

# Clean the project.
clean:
	rm -rf build
