#pragma once

#include "common.hpp"
#include "shader.hpp"
#include "window.hpp"
#include <memory>

namespace render {
struct Renderer {
	Renderer();
	void loop();

private:
	std::unique_ptr<Window> window;
	std::unique_ptr<Shader> program;
	GLFWwindow *initGLFW();
	void loadGL();
	void handleInput();
};
} // namespace render
