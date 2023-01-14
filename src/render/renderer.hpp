#pragma once

#include "common.hpp"
#include "shader.hpp"
#include <memory>

namespace render {
static const auto width = 690;
static const auto height = 420;

struct Renderer {
	Renderer();
	~Renderer();
	void loop();

private:
	GLFWwindow *window;
	std::unique_ptr<Shader> program;
	GLFWwindow *initGLFW();
	void loadGL();
	void handleInput();
};
} // namespace render
