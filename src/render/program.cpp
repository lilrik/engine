#include <iostream>
#include <render/common.hpp>
#include <render/program.hpp>
#include <stb_image/stb_image.h>
#include <util/error.hpp>

using namespace render;

Program::Program(const char *vert_data, const char *frag_data)
    : id(-1), texture_counter(0) {
  // compile shaders
  const auto vert = compileShader(true, vert_data),
             frag = compileShader(false, frag_data);

  // create the shader program
  id = glCreateProgram();

  // link shaders
  glAttachShader(id, vert);
  glAttachShader(id, frag);
  glLinkProgram(id);
  checkLinking(id);

  // destroy shaders
  glDeleteShader(vert);
  glDeleteShader(frag);
}

Program::~Program() { glDeleteShader(id); }

unsigned Program::compileShader(bool is_vert, const char *data) {
  const auto shader_type = is_vert ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;

  const auto shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &data, NULL);
  glCompileShader(shader);
  checkCompilation(shader);

  return shader;
}

void Program::Program::checkCompilation(unsigned shader) {
  if (!glIsShader(shader))
    std::cout << "error: not a valid shader" << std::endl;
  int success;
  char info_log[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, info_log);
    std::cout << "error: compilation failed: " << info_log << std::endl;
  }
}

void Program::Program::checkLinking(unsigned program) {
  int success;
  // glValidateProgram(program);
  char info_log[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, info_log);
    std::cout << "error: linking failed: " << info_log << std::endl;
  }
}

// bit dodgy
void Program::Program::setupTexture(const char *texture_name,
                                    const char *filename,
                                    bool has_transparency) {
  stbi_set_flip_vertically_on_load(true);

  // sampling
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

  // filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // mipmap filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);

  // load image
  int width = 0, height = 0, nrChannels = 0;
  // path relative to executable
  unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
  util::panicIfCond(data, "image failed to load");

  // generate texture buffer
  unsigned int texture;
  glGenTextures(1, &texture);
  glActiveTexture(GL_TEXTURE0 + texture_counter);
  // bind the texture and assign to fragment shader sampler
  glBindTexture(GL_TEXTURE_2D, texture);
  // copy image data and generate texture
  // first 0 is mipmap level (if we want to generate it manually)
  // second 0 is legacy and must always be 0
  const auto second_color_value = has_transparency ? GL_RGBA : GL_RGB;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, second_color_value,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);

  // bind the texture sampler uniform to correspondent texture number for this
  // program
  uniform(texture_name, glUniform1i, texture_counter);
  texture_counter++;
}

void Program::use() const { glUseProgram(id); }

void Program::updateModel(const glm::mat4 model) const {
  uniformM("model", glUniformMatrix4fv, 1, false, glm::value_ptr(model));
}

void Program::updateProj(const glm::mat4 projection) const {
  uniformM("projection", glUniformMatrix4fv, 1, false,
           glm::value_ptr(projection));
}
