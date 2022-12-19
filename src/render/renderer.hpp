#pragma once

#include "../../lib/glad/include/glad/glad.h"
#include "../../lib/glfw/include/GLFW/glfw3.h"

namespace render {
static const auto width = 690;
static const auto height = 420;

struct Renderer {
	Renderer();
	~Renderer();
	void loop();

private:
	GLFWwindow *window;
	unsigned program;
	GLFWwindow *initGLFW();
	void loadGL();
	void handleInput();
};
} // namespace render