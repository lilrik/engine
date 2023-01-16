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
	Window window;
	std::unique_ptr<Shader> program;
	static void loadGL();
};
} // namespace render
