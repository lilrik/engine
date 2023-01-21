#pragma once
#include "camera.hpp"
#include "common.hpp"
#include "shader.hpp"

namespace render {
struct Window {
	Window(int width = 700, int height = 700);
	~Window();
	void handleInput(const Shader *p);
	// inline functions live in hpp
	inline void swapBuffersAndPollEvents() {
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	inline bool shouldClose() { return glfwWindowShouldClose(window); }

private:
	Camera cam;
	GLFWwindow *window;
};
} // namespace render
