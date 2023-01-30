#include <render/vao.hpp>

using namespace render;

VAO::VAO(unsigned count) {
  // gen vert arr obj
  // essentially a wrapper around the vert attrib pointer configs and ebo so
  // we do not have to repeat the code or rebind every part manually. It's an
  // array of attributes (and indices if ebo defined).
  unsigned vao;
  glGenVertexArrays(count, &vao);
  id = vao;
}

void VAO::bind() const { glBindVertexArray(id); }

void VAO::newAttrib(GLuint index, GLint size, GLenum type, GLboolean normalized,
                    GLsizei stride, const GLvoid *start_offset) {
  glVertexAttribPointer(
      index /*which attrib (location in vert shader code)*/,
      size /*how many components it reads with a stride jump*/,
      type /*component type*/, normalized /*wanna normalize?*/,
      stride /*how many bytes to jump to start of next same type ele*/,
      start_offset /*start offset in bytes in vbo (cast to void*)*/);
  glEnableVertexAttribArray(
      index); // ALWAYS ENABLE BECAUSE IT IS NOT BY DEFAULT
}

void VAO::bindVBO(unsigned vbo) { glBindBuffer(GL_ARRAY_BUFFER, vbo); }

unsigned VAO::newVBO() {
  unsigned vbo;
  glGenBuffers(1, &vbo);
  return vbo;
}

unsigned VAO::newEBO(unsigned count) {
  unsigned ebo;
  glGenBuffers(count, &ebo);
  return ebo;
}

void VAO::bindEBO(unsigned ebo) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); }

void VAO::fillEBO(auto &data, GLenum usage) {
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(data), data, usage);
}
