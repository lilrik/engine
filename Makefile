CC = clang++
CFLAGS = -std=c++20 -Wall -fsanitize=address -pedantic
LIBS = lib/glad/src/glad.c lib/glfw/build/src/libglfw3.a
OSX = -framework Cocoa -framework IOKit

make:
	$(CC) $(CFLAGS) -o bin/risk *.cc $(LIBS) $(OSX)