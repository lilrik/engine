CC = clang++
CFLAGS = -std=c++20 -g -Wall -Wextra -fsanitize=address -pedantic
LIBS = -I. lib/glad/src/glad.o lib/glfw/build/src/libglfw3.a -fsanitize=address
OSX = -framework Cocoa -framework IOKit

BIN_DIR = bin
HEADERS = $(wildcard src/*.hpp) $(wildcard src/**/*.hpp)
SOURCES = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
OBJECTS = $(SOURCES:%.cpp=%.o)

# to avoid problems with files with these names
.PHONY: all

all: dirs libs prog

# make directory for object files and final binary
dirs:
	mkdir -p $(BIN_DIR)

# build glfw
libs: lib/glfw/build/src/libglfw3.a lib/glad/src/glad.o

lib/glfw/build/src/libglfw3.a:
	cd lib/glfw && cmake -S . -B build && cd build && make

lib/glad/src/glad.o:
	cd lib/glad && $(CC) -o src/glad.o -Iinclude -c src/glad.c

prog: $(OBJECTS)
	$(CC) -o $(BIN_DIR)/$@ $^ $(OSX) $(LIBS)

# build obj files
%.o: %.cpp $(HEADERS)
	$(CC) -o $@ -c $< $(CFLAGS)

run: all
	# https://stackoverflow.com/questions/69861144/get-an-error-as-a-out40780-0x1130af600-malloc-nano-zone-abandoned-due-to-in
	MallocNanoZone=0 $(BIN_DIR)/prog

clean:
	rm -rf $(BIN_DIR)
	rm $(OBJECTS) # $(OBJECTS:.o=.d) #obj dependecy files
