#include "shader.hpp"
#include "common.hpp"
#include <iostream>

using namespace render;

Shader::Shader(const char *vert_data, const char *frag_data) {
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

void Shader::checkCompilation(unsigned shader) {
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

void Shader::checkLinking(unsigned program) {
	int success;
	// glValidateProgram(program);
	char info_log[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, info_log);
		std::cout << "error: linking failed: " << info_log << std::endl;
	}
}
