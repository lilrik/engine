#include "renderer.hpp"
#include <iostream>

using namespace render;

Renderer::Renderer() {
	init_glfw();
	window = init_glfw_window();
	load_gl();
}

Renderer::~Renderer() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Renderer::loop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}

void Renderer::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Renderer::init_glfw() {
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

GLFWwindow *Renderer::init_glfw_window() {
	GLFWwindow *window = glfwCreateWindow(690, 420, "Risk", NULL, NULL);
	if (!window) {
		std::cout << "window did not init" << std::endl;
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	return window;
}

void Renderer::load_gl() {
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	gladLoadGL();
}