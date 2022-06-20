CC = clang++
CFLAGS = -std=c++20 -Wall -fsanitize=address -pedantic
LIBS = lib/glad/src/glad.c lib/glfw/build/src/libglfw3.a
OSX = -framework Cocoa -framework IOKit

make:
	$(CC) $(CFLAGS) -o bin/risk *.cc $(LIBS) $(OSX)

libs:
	cd lib/glfw && cmake -S . -B build && cd build && make && echo "\033[1mRISK built glfw\033[0m"