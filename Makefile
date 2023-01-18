CC = clang++
CFLAGS = -std=c++20 -g -Wall -Wextra -fsanitize=address -pedantic -Wpedantic -Wno-unused-parameter
LIBS = lib/glad/src/glad.o lib/glfw/build/src/libglfw3.a -fsanitize=address
OSX = -framework Cocoa -framework IOKit

EXE_NAME = prog
HEADERS = $(wildcard src/*.hpp) $(wildcard src/**/*.hpp)
SOURCES = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
OBJECTS = $(SOURCES:%.cpp=%.o)

# to avoid problems with files with these names
.PHONY: all

all: libs prog

lib/glfw/build/src/libglfw3.a:
	cd lib/glfw && cmake -S . -B build && cd build && make

lib/glad/src/glad.o: 
	cd lib/glad && $(CC) -o src/glad.o -Iinclude -c src/glad.c

libs:\
	lib/glfw/build/src/libglfw3.a\
	lib/glad/src/glad.o

%.o: %.cpp %.hpp
	$(CC) $(CFLAGS) -o $@ -c $< 

$(EXE_NAME): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(OSX) $(LIBS)

run: all
	# https://stackoverflow.com/questions/69861144/get-an-error-as-a-out40780-0x1130af600-malloc-nano-zone-abandoned-due-to-in
	MallocNanoZone=0 ./$(EXE_NAME)

clean:
	rm $(EXE_NAME)
	rm $(OBJECTS) # $(OBJECTS:.o=.d) #obj dependecy files
