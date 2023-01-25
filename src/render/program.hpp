#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <render/common.hpp>
#include <util/error.hpp>

namespace render {
struct Program {
  Program(const char *vert_data, const char *frag_data);
  ~Program();
  // we must not accidentally copy in order to avoid deleting program early
  Program(const Program &other) = delete;

  void use() const;

  // enables automatically
  void newAttrib(GLuint index, GLint size, GLenum type, GLboolean normalized,
                 GLsizei stride, const GLvoid *start_offset) const;

  void updateModel(const glm::mat4 model) const;
  void updateProj(const glm::mat4 projection) const;

  void setupTexture(const char *texture_name, const char *filename,
                    bool has_transparency);

  template <typename F, typename... T>
  void uniform(const char *name, F func, T... args) {
    // get current id to reset it after (THIS IS SLOW BECAUSE WE QUERY OPENGL
    // STATE)
    // GLint old_id = 0;
    // glGetIntegerv(GL_CURRENT_PROGRAM, &old_id);

    // use(); // must set

    const auto location = glGetUniformLocation(id, name);
    if (location == -1) {
      std::cout << "error: uniform " << name << " not found" << std::endl;
      return;
    }
    func(location, args...);

    // glUseProgram(old_id); // reset old program
  }

  template <typename F, typename... T>
  void uniformM(const char *name, F func, unsigned num_matrices, bool transpose,
                T... args) const {
    // get current id to reset it after (THIS IS SLOW BECAUSE WE QUERY OPENGL
    // STATE)
    // GLint old_id = 0;
    // glGetIntegerv(GL_CURRENT_PROGRAM, &old_id);

    // use(); // must set

    const auto location = glGetUniformLocation(id, name);
    if (location == -1) {
      std::cout << "error: uniform " << name << " not found " << id
                << std::endl;
      return;
    }
    func(location, num_matrices, transpose ? GL_TRUE : GL_FALSE, args...);

    // glUseProgram(old_id); // reset old program
  }

private:
  // shader program id
  unsigned id;
  unsigned texture_counter;

  unsigned compileShader(bool is_vert, const char *data);
  void checkCompilation(unsigned shader);
  void checkLinking(unsigned program);
};
} // namespace render
