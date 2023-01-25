#pragma once
#include <array>
#include <render/camera.hpp>
#include <render/common.hpp>
#include <render/program.hpp>

namespace render {
struct Window {
  float width, height;

  // TODO: eventualmente tornar isto numa lista de observers que dao handle de
  // input currently set camera
  Camera *cam;

  Window(int width = 1000, int height = 700, Camera *cam = nullptr);
  ~Window();
  // inline functions live in hpp
  inline void swapBuffersAndPollEvents() {
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  inline bool shouldClose() { return glfwWindowShouldClose(window); }

  inline void handleAllKeyInput() {
    cam->handleKeyInput(keys_pressed);
    handleKeyInput();
  }

  void handleKeyInput();

private:
  std::array<int, 5> keys_pressed = {0, 0, 0, 0, 0};
  GLFWwindow *window;
};
} // namespace render
