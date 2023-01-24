#include <iostream>
#include <render/common.hpp>
#include <render/shader.hpp>
#include <stb_image/stb_image.h>
#include <util/error.hpp>

using namespace render;

Shader::Shader(const char *vert_data, const char *frag_data)
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

Shader::~Shader() { glDeleteShader(id); }

unsigned Shader::compileShader(bool is_vert, const char *data) {
  const auto shader_type = is_vert ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;

  const auto shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &data, NULL);
  glCompileShader(shader);
  checkCompilation(shader);

  return shader;
}

void Shader::Shader::checkCompilation(unsigned shader) {
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

void Shader::Shader::checkLinking(unsigned program) {
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
void Shader::Shader::setupTexture(const char *texture_name,
                                  const char *filename, bool has_transparency) {
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

void Shader::use() const { glUseProgram(id); }

unsigned Shader::newVAO(unsigned count) {
  // gen vert arr obj
  // essentially a wrapper around the vert attrib pointer configs and ebo so
  // we do not have to repeat the code or rebind every part manually. It's an
  // array of attributes (and indices if ebo defined).
  unsigned vao;
  glGenVertexArrays(count, &vao);
  return vao;
}

void Shader::bindVAO(unsigned vao) { glBindVertexArray(vao); }

unsigned Shader::newEBO(unsigned count) {
  unsigned ebo;
  glGenBuffers(count, &ebo);
  return ebo;
}

void Shader::bindEBO(unsigned ebo) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

void Shader::fillEBO(auto &data, GLenum usage) {
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(data), data, usage);
}

unsigned Shader::newVBO() {
  unsigned vbo;
  glGenBuffers(1, &vbo);
  return vbo;
}

void Shader::bindVBO(unsigned vbo) { glBindBuffer(GL_ARRAY_BUFFER, vbo); }

void Shader::newAttrib(GLuint index, GLint size, GLenum type,
                       GLboolean normalized, GLsizei stride,
                       const GLvoid *start_offset) const {
  glVertexAttribPointer(
      index /*which attrib (location in vert shader code)*/,
      size /*how many components it reads with a stride jump*/,
      type /*component type*/, normalized /*wanna normalize?*/,
      stride /*how many bytes to jump to start of next same type ele*/,
      start_offset /*start offset in bytes in vbo (cast to void*)*/);
  glEnableVertexAttribArray(
      index); // ALWAYS ENABLE BECAUSE IT IS NOT BY DEFAULT
}

void Shader::updateModel(const glm::mat4 model) const {
  uniformM("model", glUniformMatrix4fv, 1, false, glm::value_ptr(model));
}

void Shader::updateProj(const glm::mat4 projection) const {
  uniformM("projection", glUniformMatrix4fv, 1, false,
           glm::value_ptr(projection));
}
