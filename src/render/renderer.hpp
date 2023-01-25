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
  void loop();

  static unsigned newVAO(unsigned count = 1);
  static void bindVAO(unsigned vao);

  static unsigned newEBO(unsigned count = 1);
  static void bindEBO(unsigned ebo);
  static void fillEBO(auto &data, GLenum usage);

  static unsigned newVBO();
  static void bindVBO(unsigned vbo);
  // reference to prevent pointer decay
  static void fillVBO(auto &data, GLenum usage);

private:
  // window inits glfw (and its context) and ogl so it must come first
  Window window;
  std::array<Program, num_programs> programs{
      Program(main_vert, main_frag),
      Program(light_vert, light_frag),
  };
  Camera cam;
};
} // namespace render
