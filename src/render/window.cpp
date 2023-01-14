#include "window.hpp"
#include <iostream>

using namespace render;

Window::Window(int width, int height) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	if (!glfwInit())
		std::cout << "error: glfw did not init" << std::endl;
	glfwSetErrorCallback(nullptr);

	// init window
	GLFWwindow *window = glfwCreateWindow(width, height, "Risk", NULL, NULL);
	if (!window) {
		std::cout << "error: window did not init" << std::endl;
		exit(1);
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, []([[maybe_unused]] GLFWwindow *w, int width, int height) {
		glViewport(0, 0, width, height);
	});
	// glfwSetKeyCallback(window, key_callback);
	this->window = window;
}

Window::~Window() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::handleInput() {
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
