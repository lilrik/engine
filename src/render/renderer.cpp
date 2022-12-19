#include "renderer.hpp"
#include <iostream>

using namespace render;

Renderer::Renderer() {
	initGLFW();
	window = initGLFWWindow();
	loadGL();
}

Renderer::~Renderer() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Renderer::loop() {
	while (!glfwWindowShouldClose(window)) {
		handleInput();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Renderer::handleInput() {
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void Renderer::initGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	if (!glfwInit())
		std::cout << "glfw did not init" << std::endl;
	glfwSetErrorCallback(nullptr);
}

GLFWwindow *Renderer::initGLFWWindow() {
	GLFWwindow *window = glfwCreateWindow(width, height, "Risk", NULL, NULL);
	if (!window) {
		std::cout << "window did not init" << std::endl;
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(
			window, [](GLFWwindow *w, int width, int height) { glViewport(0, 0, width, height); });
	// glfwSetKeyCallback(window, key_callback);
	return window;
}

void Renderer::loadGL() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "error: GLAD did not load GL fns" << std::endl;
		return;
	}
	gladLoadGL();
	glViewport(0, 0, width, height);
}