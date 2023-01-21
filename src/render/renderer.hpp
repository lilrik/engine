#pragma once

#include "camera.hpp"
#include "common.hpp"
#include "shader.hpp"
#include "window.hpp"
#include <memory>

namespace render {
struct Renderer {
	Renderer();
	void loop();

private:
	Camera cam;
	Window window;
	std::unique_ptr<Shader> program;
	static void loadGL();
};
} // namespace render
