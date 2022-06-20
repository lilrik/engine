#pragma once

#include "lib/glad/include/glad/glad.h"
#include "lib/glfw/include/GLFW/glfw3.h"

namespace render {
struct Renderer {
	Renderer();
	~Renderer();
	void loop();

private:
	GLFWwindow *window;
	void init_glfw();
	GLFWwindow *init_glfw_window();
	void load_gl();
	static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
};
} // namespace render