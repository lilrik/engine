#pragma once
#include "common.hpp"
#include <iostream>

namespace render {
struct Shader {
	static constexpr auto default_vert = R"(
		#version 330 core
		layout (location = 0) in vec3 aPos; // location so we don't have to query for it with glAttribLoc
		layout (location = 1) in vec3 aCol; // location so we don't have to query for it with glAttribLoc

		out vec4 vertexColor;

		void main() {
			gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
			vertexColor = vec4(aCol.x, aCol.y, aCol.z, 1.0);
		}
	)";

	static constexpr auto default_frag = R"(
		#version 330 core
		out vec4 FragColor;

		in vec4 vertexColor;

		//uniform vec4 ourColor; // acts as global variable of a single shader *program* and can be declared at any time
		// If you declare a uniform that isn't used anywhere 
		// in your GLSL code the compiler will silently remove 
		// the variable from the compiled version which is the 
		// cause for several frustrating errors; keep this in mind!

		void main() {
			FragColor = vertexColor;
		} 
	)";

	unsigned id;

	Shader(const char *vert_data = default_vert, const char *frag_data = default_frag);
	~Shader();
	template <typename F, typename... T> void uniform(const char *name, F func, T... args) {
		const auto location = glGetUniformLocation(id, "ourColor");
		if (location == -1) {
			std::cout << "error: uniform " << name << "not found" << std::endl;
			return;
		}
		// get current id to reset it after (THIS IS SLOW BECAUSE WE QUERY OPENGL STATE)
		GLint old_id = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &old_id);

		use(); // must set
		func(location, args...);

		glUseProgram(old_id); // reset old program
	}
	inline void use() { glUseProgram(id); }

private:
	unsigned compileShader(bool is_vert, const char *data);
	void checkCompilation(unsigned shader);
	void checkLinking(unsigned program);
};
} // namespace render
