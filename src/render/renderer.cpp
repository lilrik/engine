#include <cmath>
#include <render/renderer.hpp>
#include <render/shader.hpp>
#include <render/window.hpp>
#include <util/error.hpp>
// #include <ranges> eventually figure out how to make this work
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define LOG_FPS()                                                              \
  if (((int)cam.prev_frame_time) % 5 == 0)                                     \
    std::cout << pow(cam.delta_time, -1) << std::endl;

using namespace render;

// clang-format off
static constexpr float cube_verts[] = {            
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
                                       
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                                       
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                                       
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                                       
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                                       
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	};

static constexpr glm::vec3 cubes_pos[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)
	};
// clang-format on

void renderCubes(const Shader &program) {
  // for each frame we iterate over the array of the cube positions and, using a
  // new model matrix made from the cube positions translation + index based
  // rotation, draw the one cube (12 triangles aka 36 verts) but in a
  // new pos (because of the new model matrix)
  for (auto i = 0; i < 10; i++) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, cubes_pos[i]);
    float angle = 40.0f * i;
    model =
        glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
    program.updateModel(model);

    // first is mode
    // second is first index in the array we specified in attributes (the offset
    // just says where the array starts in the data and here it actually says
    // which part of the array we want it to be the first) third is num verts
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }
}

Renderer::Renderer() : cam(Camera(program)) {
  // inject input observers into window
  window.cam = &cam;
}

void Renderer::loop() {
  glViewport(0, 0, window.width, window.height);
  // must clear this buffer (like the color buffer) every frame (done below)
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  program.use();

  const auto vao = program.newVAO();
  program.bindVAO(vao);

  const auto vbo = program.newVBO();
  program.bindVBO(vbo);
  program.fillVBO(cube_verts, GL_STATIC_DRAW);
  program.newAttrib(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  program.newAttrib(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                    (void *)(3 * sizeof(float)));

  program.setupTexture("texture0", "rsc/container.jpg", false);
  program.setupTexture("texture1", "rsc/smiley.png", true);

  program.updateProj(glm::perspective(
      glm::radians(cam.fov), window.width / window.height, 0.1f, 100.0f));
  cam.updateLookAt();

  while (!window.shouldClose()) {
    util::panicIfErr(glGetError(), "opengl error");
    // LOG_FPS();
    cam.adjustSpeed();
    window.handleAllKeyInput();

    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render
    renderCubes(program);

    // poll events
    window.swapBuffersAndPollEvents();
  }
}

// glm::mat4 makeTransMat() {
//   // scale and rotate container
//   // attention: glm expects radians
//   // attention: glm expects a unit vector as the rotation axis
//   glm::mat4 trans = glm::mat4(1.0f);
//   trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
//   trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
//   return trans;
// }
//
// glm::mat4 makeRotMat() {
//   glm::mat4 trans = glm::mat4(1.0f);
//   trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
//   trans = glm::rotate(trans, (float)glfwGetTime() /* angle over time*/,
//                       glm::vec3(0.0f, 0.0f, 1.0f));
//   return trans;
// }
