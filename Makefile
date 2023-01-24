CXX = clang++
CXXFLAGS = -std=c++20 -g -fsanitize=address\
					 -Wall -Wextra -pedantic -Wpedantic -Wno-unused-parameter\
					 -Ilib/glad/include -Ilib/glfw/include -Ilib/stb_image/include -Ilib/glm\
					 -Isrc
LIBS = lib/glad/src/glad.o lib/stb_image/stb_image.o lib/glfw/build/src/libglfw3.a
OSX = -framework Cocoa -framework IOKit

EXE_NAME = prog
HEADERS = $(wildcard src/*.hpp) $(wildcard src/**/*.hpp)
SOURCES = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
OBJECTS = $(SOURCES:%.cpp=%.o)
DEPS = $(SOURCES:%.cpp=%.d) # DEPENDENCIES IS A RESERVED NAME WTF?

$(EXE_NAME): $(OBJECTS) $(LIBS)
	@echo $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(OSX)

run: $(EXE_NAME)
	# https://stackoverflow.com/questions/69861144/get-an-error-as-a-out40780-0x1130af600-malloc-nano-zone-abandoned-due-to-in
	MallocNanoZone=0 ./$(EXE_NAME)

lib/glfw/build/src/libglfw3.a:
	cd lib/glfw && cmake -S . -B build && cd build && make

lib/glad/src/glad.o: 
	cd lib/glad && clang -o src/glad.o -Iinclude -c src/glad.c

lib/stb_image/stb_image.o: 
	cd lib/stb_image && clang -o stb_image.o -c stb_image.c

-include $(DEPS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c -MMD $< 

clean:
	rm $(DEPS)
	rm $(OBJECTS)
	rm $(EXE_NAME)
