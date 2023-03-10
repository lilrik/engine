#pragma once

#include <render/camera.hpp>
#include <render/common.hpp>
#include <render/program.hpp>
#include <render/shaders.hpp>
#include <render/window.hpp>
#include <util/error.hpp>

namespace render {
struct Renderer {
  Renderer();
  void draw();

private:
  // window inits glfw (and its context) and gl so it must initialize first
  Window window;
  std::array<Program, num_programs> programs{
      Program(main_vert, main_frag),
      Program(light_vert, light_frag),
  };
  Camera cam;
};
} // namespace render
