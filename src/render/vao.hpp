#pragma once

#include <render/common.hpp>

namespace render {
struct VAO {
  int id;

  VAO(unsigned count = 1);
  void bind() const;

  static unsigned newEBO(unsigned count = 1);
  static void bindEBO(unsigned ebo);
  static void fillEBO(auto &data, GLenum usage);

  static unsigned newVBO();
  static void bindVBO(unsigned vbo);
  // reference to prevent pointer decay
  static void fillVBO(auto &data, GLenum usage) {
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, usage);
  }

  // enables automatically
  static void newAttrib(GLuint index, GLint size, GLenum type,
                        GLboolean normalized, GLsizei stride,
                        const GLvoid *start_offset);
};
} // namespace render
