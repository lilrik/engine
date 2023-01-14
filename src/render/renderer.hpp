#pragma once

#include "common.hpp"
#include "shader.hpp"
#include "window.hpp"

namespace render {
struct Renderer {
	Renderer();
	~Renderer();
	void loop();

private:
	Window *window;
	Shader *program;
	GLFWwindow *initGLFW();
	void loadGL();
	void handleInput();
};
} // namespace render
