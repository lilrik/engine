#pragma once

#include <memory>
#include <render/camera.hpp>
#include <render/common.hpp>
#include <render/shader.hpp>
#include <render/window.hpp>
#include <util/error.hpp>

namespace render {
struct Renderer {
  Renderer();
  void loop();

private:
  // window inits glfw (and its context) and ogl so it must come first
  Window window;
  Shader program;
  Camera cam;
};
} // namespace render
