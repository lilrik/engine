#include <cmath>
#include <render/renderer.hpp>
#include <render/window.hpp>
#include <util/error.hpp>
// #include <ranges> eventually figure out how to make this work
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#define LOG_FPS()                                                              \
  if (((int)cam.prev_frame_time) % 5 == 0)                                     \
    std::cout << pow(cam.delta_time, -1) << std::endl;

using namespace render;

// clang-format off
static constexpr float cube_verts[] = {            
		// positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
};

static constexpr glm::vec3 cubes_pos[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)
	};
// clang-format on

static constexpr glm::vec3 point_lights_pos[] = {
    glm::vec3(0.7f, 0.2f, 2.0f),
    glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f, 2.0f, -12.0f),
    glm::vec3(0.0f, 0.0f, -3.0f),
};

unsigned Renderer::newVAO(unsigned count) {
  // gen vert arr obj
  // essentially a wrapper around the vert attrib pointer configs and ebo so
  // we do not have to repeat the code or rebind every part manually. It's an
  // array of attributes (and indices if ebo defined).
  unsigned vao;
  glGenVertexArrays(count, &vao);
  return vao;
}

void Renderer::bindVAO(unsigned vao) { glBindVertexArray(vao); }

unsigned Renderer::newEBO(unsigned count) {
  unsigned ebo;
  glGenBuffers(count, &ebo);
  return ebo;
}

void Renderer::bindEBO(unsigned ebo) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

void Renderer::fillEBO(auto &data, GLenum usage) {
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(data), data, usage);
}

unsigned Renderer::newVBO() {
  unsigned vbo;
  glGenBuffers(1, &vbo);
  return vbo;
}

void Renderer::bindVBO(unsigned vbo) { glBindBuffer(GL_ARRAY_BUFFER, vbo); }

void Renderer::fillVBO(auto &data, GLenum usage) {
  glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, usage);
}

static void renderLight(const Program &program, unsigned vao) {
  program.use();
  Renderer::bindVAO(vao);
  for (const auto &pos : point_lights_pos) {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    program.updateModel(model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }
}

static void renderCubes(const Program &program, unsigned vao) {
  program.use();
  Renderer::bindVAO(vao);
  // for each frame we iterate over the array of the cube positions and, using a
  // new model matrix made from the cube positions translation + index based
  // rotation, draw the one cube (12 triangles aka 36 verts) but in a
  // new pos (because of the new model matrix)
  for (auto i = 0; i < 10; i++) {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, cubes_pos[i]);
    model = glm::rotate(model, glm::radians(40.0f * i),
                        glm::vec3(1.0f, 0.3f, 0.5f));
    program.updateModel(model);

    // first is mode
    // second is first index in the array we specified in attributes (the offset
    // just says where the array starts in the data and here it actually says
    // which part of the array we want it to be the first) third is num verts
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }
}

Renderer::Renderer() : cam(Camera(programs)) {
  // inject input observers into window
  window.cam = &cam;
}

void Renderer::loop() {
  glViewport(0, 0, window.width, window.height);
  // must clear this buffer (like the color buffer) every frame (done below)
  glEnable(GL_DEPTH_TEST);
  // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // TODO: VAOs are not associated with programs and therefore should not be a
  // method
  // TODO: all shader methods should be marked const
  // TODO: attributes should be VAO::newAttrib

  // TODO: gen two at once
  const auto vao1 = newVAO();
  bindVAO(vao1);
  const auto vao2 = newVAO();
  //  put data in buffers (it's the same)
  const auto vbo = newVBO();
  bindVBO(vbo);
  fillVBO(cube_verts, GL_STATIC_DRAW);

  auto &program = programs[0], &light_program = programs[1];

  const auto light_pos = glm::vec3(1.2f, 1.0f, 2.0f);

  // setup cubes program
  program.use();
  program.newAttrib(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  program.newAttrib(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                    (void *)(3 * sizeof(float)));
  program.newAttrib(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                    (void *)(6 * sizeof(float)));

  program.setupTexture("material.diffuse", "rsc/container2.png", true);
  program.setupTexture("material.specular", "rsc/container2specular.png", true);
  program.uniform("material.shininess", glUniform1f, 32.0f);

  program.uniform("spotlight.ambient", glUniform3f, 1.0f, 1.0f, 1.0f);
  program.uniform("spotlight.diffuse", glUniform3f, 0.5f, 0.5f, 0.5f);
  program.uniform("spotlight.specular", glUniform3f, 1.0f, 1.0f, 1.0f);
  program.uniform("spotlight.cutOff", glUniform1f,
                  glm::cos(glm::radians(12.5f)));
  program.uniform("spotlight.outerCutOff", glUniform1f,
                  glm::cos(glm::radians(17.5f)));
  program.uniform("spotlight.constant", glUniform1f, 1.0f);
  program.uniform("spotlight.linear", glUniform1f, 0.09f);
  program.uniform("spotlight.quadratic", glUniform1f, 0.032f);

  program.uniform("dirLight.direction", glUniform3f, 0.0f, -1.0f, 0.0f);
  program.uniform("dirLight.ambient", glUniform3f, 0.2f, 0.2f, 0.2f);
  program.uniform("dirLight.diffuse", glUniform3f, 0.5f, 0.5f, 0.5f);
  program.uniform("dirLight.specular", glUniform3f, 1.0f, 1.0f, 1.0f);

#define STRFMT(index, name)                                                    \
  (std::string("pointLights[") + std::to_string(index) + std::string("].") +   \
   std::string(name))                                                          \
      .c_str()

  for (auto i = 0; i < 4; i++) {
    program.uniform(STRFMT(i, "position"), glUniform3fv, 1,
                    glm::value_ptr(point_lights_pos[i]));
    program.uniform(STRFMT(i, "ambient"), glUniform3f, 0.2f, 0.2f, 0.2f);
    program.uniform(STRFMT(i, "diffuse"), glUniform3f, 0.5f, 0.5f, 0.5f);
    program.uniform(STRFMT(i, "specular"), glUniform3f, 1.0f, 1.0f, 1.0f);
    program.uniform(STRFMT(i, "constant"), glUniform1f, 1.0f);
    program.uniform(STRFMT(i, "linear"), glUniform1f, 0.09f);
    program.uniform(STRFMT(i, "quadratic"), glUniform1f, 0.032f);
  }

  // setup light program
  // vao for the light cube
  bindVAO(vao2);
  light_program.use();
  bindVBO(vbo);
  light_program.newAttrib(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
  light_program.updateProj(glm::perspective(
      glm::radians(cam.fov), window.width / window.height, 0.1f, 100.0f));
  auto model = glm::mat4(1.0f);
  model = glm::translate(model, light_pos);
  model = glm::scale(model, glm::vec3(0.2f));
  light_program.updateModel(model);

  // updates both these programs
  cam.updateLookAt();

  while (!window.shouldClose()) {
    util::panicIfErr(glGetError(), "opengl error");
    // LOG_FPS();
    cam.adjustSpeed();
    window.handleAllKeyInput();

    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render light
    renderLight(light_program, vao2);

    // render cubes
    renderCubes(program, vao1);

    program.uniform("spotlight.frontDir", glUniform3fv, 1,
                    glm::value_ptr(cam.front));

    // in the loop so it changes with window resize
    // (lowkey should be in callback in window)
    program.updateProj(glm::perspective(
        glm::radians(cam.fov), window.width / window.height, 0.1f, 100.0f));
    program.uniform("viewPos", glUniform3fv, 1, glm::value_ptr(cam.pos));

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
