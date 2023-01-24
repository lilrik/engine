#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <render/common.hpp>

namespace render {
struct Shader {
  static constexpr auto default_vert = R"(
		#version 330 core
		// location so we don't have to query for it with glAttribLoc
		layout (location = 0) in vec3 aPos;
		//layout (location = 1) in vec3 aCol;
		layout (location = 1) in vec2 aTexCoord;

		out vec2 texCoord;

		uniform mat4 transform;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		void main() {
			//gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
			gl_Position = projection * view * model * vec4(aPos, 1.0);
			texCoord = aTexCoord;
		}
	)";

  static constexpr auto default_frag = R"(
		#version 330 core
		out vec4 FragColor;

		in vec2 texCoord;

		// so the frag shader can access the texture
		uniform sampler2D texture0;
		uniform sampler2D texture1;

		//uniform vec4 ourColor; // acts as global variable of a single shader *program* and can be declared at any time
		// If you declare a uniform that isn't used anywhere 
		// in your GLSL code the compiler will silently remove 
		// the variable from the compiled version which is the 
		// cause for several frustrating errors; keep this in mind!

		void main() {
			// built-in function that actually does the sampling and filtering
			//FragColor = vec4(1.0, 0.0, 0.0, 0.0);
			//FragColor = texture(texture0, texCoord);
			// TRANSPARENCY NOT WORKING; FIX THIS
			FragColor = mix(texture(texture0, texCoord), texture(texture1, texCoord), 0.2);
		} 
	)";

  Shader(const char *vert_data = default_vert,
         const char *frag_data = default_frag);
  ~Shader();

  void use() const;

  unsigned newVAO(unsigned count = 1);
  void bindVAO(unsigned vao);

  unsigned newEBO(unsigned count = 1);
  void bindEBO(unsigned ebo);
  void fillEBO(auto &data, GLenum usage);

  unsigned newVBO();
  void bindVBO(unsigned vbo);
  // reference to prevent pointer decay
  void fillVBO(auto &data, GLenum usage) {
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, usage);
  }

  // enables automatically
  void newAttrib(GLuint index, GLint size, GLenum type, GLboolean normalized,
                 GLsizei stride, const GLvoid *start_offset) const;

  void updateModel(const glm::mat4 model) const;
  void updateProj(const glm::mat4 projection) const;

  void setupTexture(const char *texture_name, const char *filename,
                    bool has_transparency);

  template <typename F, typename... T>
  void uniform(const char *name, F func, T... args) {
    const auto location = glGetUniformLocation(id, name);
    if (location == -1) {
      std::cout << "error: uniform " << name << " not found" << std::endl;
      return;
    }
    // get current id to reset it after (THIS IS SLOW BECAUSE WE QUERY OPENGL
    // STATE)
    GLint old_id = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &old_id);

    use(); // must set
    func(location, args...);

    glUseProgram(old_id); // reset old program
  }

  template <typename F, typename... T>
  void uniformM(const char *name, F func, unsigned num_matrices, bool transpose,
                T... args) const {
    const auto location = glGetUniformLocation(id, name);
    if (location == -1) {
      std::cout << "error: uniform " << name << " not found" << std::endl;
      return;
    }
    // get current id to reset it after (THIS IS SLOW BECAUSE WE QUERY OPENGL
    // STATE)
    GLint old_id = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &old_id);

    use(); // must set
    func(location, num_matrices, transpose ? GL_TRUE : GL_FALSE, args...);

    glUseProgram(old_id); // reset old program
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
