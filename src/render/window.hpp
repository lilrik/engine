#pragma once
#include "common.hpp"

namespace render {
struct Window {
	Window(int width = 700, int height = 700);
	~Window();
	void handleInput();
	// inline functions live in hpp
	inline void swapBuffersAndPollEvents() {
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	inline bool shouldClose() { return glfwWindowShouldClose(window); }

private:
	GLFWwindow *window;
};
} // namespace render
