#pragma once
#include "common.hpp"
#include <iostream>

#define SHADER_CHECK_IMAGE_LOADED(DATA_POINTER)                                                    \
	do {                                                                                             \
		if (DATA_POINTER == nullptr) {                                                                 \
			std::cout << "error: failed to load image: " << stbi_failure_reason() << std::endl;          \
			exit(1);                                                                                     \
		}                                                                                              \
	} while (0)

namespace render {

struct Shader {
	static constexpr auto default_vert = R"(
		#version 330 core
		// location so we don't have to query for it with glAttribLoc
		layout (location = 0) in vec3 aPos;
		layout (location = 1) in vec3 aCol;
		layout (location = 2) in vec2 aTexCoord;

		out vec4 vertexColor;
		out vec2 texCoord;

		uniform mat4 transform;

		void main() {
			gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
			vertexColor = vec4(aCol.x, aCol.y, aCol.z, 1.0);
			texCoord = aTexCoord;
		}
	)";

	static constexpr auto default_frag = R"(
		#version 330 core
		out vec4 FragColor;

		in vec4 vertexColor;
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
			FragColor = mix(texture(texture0, texCoord), texture(texture1, texCoord), 0.2);
		} 
	)";

	Shader(const char *vert_data = default_vert, const char *frag_data = default_frag);
	~Shader();
	template <typename F, typename... T> void uniform(const char *name, F func, T... args) {
		const auto location = glGetUniformLocation(id, name);
		if (location == -1) {
			std::cout << "error: uniform " << name << " not found" << std::endl;
			return;
		}
		// get current id to reset it after (THIS IS SLOW BECAUSE WE QUERY OPENGL STATE)
		GLint old_id = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &old_id);

		use(); // must set
		func(location, args...);

		glUseProgram(old_id); // reset old program
	}
	template <typename F, typename... T>
	void uniformM(const char *name, F func, unsigned num_matrices, bool transpose, T... args) {
		const auto location = glGetUniformLocation(id, name);
		if (location == -1) {
			std::cout << "error: uniform " << name << " not found" << std::endl;
			return;
		}
		// get current id to reset it after (THIS IS SLOW BECAUSE WE QUERY OPENGL STATE)
		GLint old_id = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &old_id);

		use(); // must set
		func(location, num_matrices, transpose ? GL_TRUE : GL_FALSE, args...);

		glUseProgram(old_id); // reset old program
	}
	inline void use() { glUseProgram(id); }
	void setupTexture(const char *texture_name, const char *filename, bool has_transparency);

private:
	// shader program id
	unsigned id;
	unsigned texture_counter;

	unsigned compileShader(bool is_vert, const char *data);
	void checkCompilation(unsigned shader);
	void checkLinking(unsigned program);
};
} // namespace render
