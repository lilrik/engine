#include <render/common.hpp>
#include <util/error.hpp>

namespace util {
static void loadGLFW() {
  panicIfCond(glfwInit(), "glfw did not init");
  glfwSetErrorCallback(util::panicIfCond);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  // to avoid mac retina problems - framebuffer vs window
  glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
#endif
}
static void loadGL() {
  panicIfCond(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress),
              "GLAD could not load GL fns");
  gladLoadGL();
}
} // namespace util
