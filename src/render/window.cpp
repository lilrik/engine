#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <render/window.hpp>
#include <util/error.hpp>
#include <util/load.hpp>

using namespace render;

#define CAM_POINTER static_cast<Window *>(glfwGetWindowUserPointer(window))->cam

Window::Window(int width, int height, Camera *cam)
    : width(width), height(height), cam(nullptr) {
  util::loadGLFW();

  // init window
  GLFWwindow *window = glfwCreateWindow(width, height, "Engine", NULL, NULL);
  util::panicIfCond(window, "window did not init");
  glfwMakeContextCurrent(window);

  util::loadGL();

  // associate glfw window pointer with user defined data pointer
  glfwSetWindowUserPointer(window, this);
  this->window = window;

  // callbacks
  glfwSetFramebufferSizeCallback(
      window, []([[maybe_unused]] GLFWwindow *window, int width, int height) {
        auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
        self->width = width;
        self->height = height;
        glViewport(0, 0, width, height);
      });

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window,
                           [](GLFWwindow *window, double xpos, double ypos) {
                             // do not move the camera when the window is not
                             // focused
                             if (!glfwGetWindowAttrib(window, GLFW_FOCUSED))
                               return;

                             auto cam = CAM_POINTER;
                             cam->handleMouseInput(xpos, ypos);
                           });

  glfwSetWindowFocusCallback(window, [](GLFWwindow *window, int focused) {
    const auto cam = CAM_POINTER;
    if (focused)
      glfwSetCursorPos(window, cam->prev_pos.mouse_x, cam->prev_pos.mouse_y);
  });

  glfwSetScrollCallback(window,
                        [](GLFWwindow *window, double xoffset, double yoffset) {
                          auto cam = CAM_POINTER;
                          cam->handleScrollInput(xoffset, yoffset);
                        });

  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode,
                                int action, int mods) {
    auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));

    for (int &key_pressed : self->keys_pressed) {
      if (key_pressed == 0 && action == GLFW_PRESS) {
        key_pressed = key;
        break;
      }

      if (key_pressed == key && action == GLFW_RELEASE) {
        key_pressed = 0;
        break;
      }
    }
  });
}

void Window::handleKeyInput() {
  for (int key : keys_pressed)
    switch (key) {
    case GLFW_KEY_Q:
      glfwSetWindowShouldClose(window, true);
      break;
    case GLFW_KEY_L:
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      break;
    case GLFW_KEY_F:
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      break;
    }
}

Window::~Window() {
  glfwDestroyWindow(window);
  glfwTerminate();
}
