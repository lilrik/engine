#include <iostream>

#include "lib/glad/include/glad/glad.h"
#include "lib/glfw/include/GLFW/glfw3.h"

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void init_glfw() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	if (!glfwInit())
		std::cout << "glfw did not init" << std::endl;
	glfwSetErrorCallback([](int err, const char *description) noexcept {
		std::cout << "err " << err << " " << description << std::endl;
	});
}

GLFWwindow *init_glfw_window() {
	GLFWwindow *window = glfwCreateWindow(690, 420, "Risk", NULL, NULL);
	if (!window) {
		std::cout << "window did not init" << std::endl;
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	return window;
}

void load_gl() {
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	gladLoadGL();
}

void terminate_glfw(GLFWwindow *window) {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void render_loop(GLFWwindow *window) {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}

int main() {
	init_glfw();
	auto window = init_glfw_window();
	load_gl();

	render_loop(window);

	terminate_glfw(window);
	return 0;
}