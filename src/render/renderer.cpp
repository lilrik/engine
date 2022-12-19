#include "renderer.hpp"
#include <iostream>

using namespace render;

void checkCompilation(unsigned shader) {
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

void checkLinking(unsigned program) {
	int success;
	// glValidateProgram(program);
	char info_log[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, info_log);
		std::cout << "error: linking failed: " << info_log << std::endl;
	}
}

unsigned buffers() {
	const float verts[] = {
			-0.5f, -0.5f, 0.0f, // bl
			0.5f,	 -0.5f, 0.0f, // br
			-0.5f, 0.5f,	0.0f, // tl
			0.5f,	 0.5f,	0.0f, // tr
	};

	unsigned int indices[] = {
			0, 1,
			2, // t1
			2, 3,
			1 // t2
	};

	// gen vert arr obj
	// essentially a wrapper around the vert attrib pointer configs and ebo so we
	// do not have to repeat the code or rebind every part manually. It's an array
	// of attributes.
	unsigned vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// gen ele buf obj
	unsigned ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
							 GL_STATIC_DRAW);

	// gen vert buf obj
	unsigned vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// copy data
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	// how to interpret data (this applies to the currently bound buffer)
	glVertexAttribPointer(
			0 /*which attribute (the location specified in vert shader)*/,
			3 /*size of said attribute*/, GL_FLOAT /*type of the components*/,
			GL_FALSE /*normalize*/,
			3 * sizeof(float) /*stride (0 in packed structs does it automatically)*/,
			(void *)0 /*start offset*/);
	// disabled by default
	glEnableVertexAttribArray(0);

	return vao;
}

unsigned vertexShader() {
	const auto vert_shader_data = R"(
		#version 330 core
		layout (location = 0) in vec3 aPos;

		void main() {
			gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
		}
	)";

	// compile shader
	const auto vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vert_shader_data, NULL);
	glCompileShader(vert_shader);
	checkCompilation(vert_shader);

	return vert_shader;
}

unsigned fragmentShader() {
	const auto frag_shader_data = R"(
		#version 330 core
		out vec4 FragColor;

		void main() {
			FragColor = vec4(0.0f, 1.0f, 0.2f, 1.0f);
		} 
	)";

	// compile shader
	const auto frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &frag_shader_data, NULL);
	glCompileShader(frag_shader);
	checkCompilation(frag_shader);

	return frag_shader;
}

unsigned shaderProgram(unsigned vert, unsigned frag) {
	const auto program = glCreateProgram();

	// link
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);
	checkLinking(program);

	// destroy
	glDeleteShader(vert);
	glDeleteShader(frag);

	return program;
}

Renderer::Renderer() {
	window = initGLFW();
	loadGL();
	buffers();
	program = shaderProgram(vertexShader(), fragmentShader());
}

Renderer::~Renderer() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Renderer::loop() {
	while (!glfwWindowShouldClose(window)) {
		handleInput();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program);
		const auto vao = buffers();
		glBindVertexArray(vao);

		//  bind ebo if needed but usually not because it is also wrapped by the vao
		//  second arg is num of verts!
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Renderer::handleInput() {
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

GLFWwindow *Renderer::initGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	if (!glfwInit())
		std::cout << "error: glfw did not init" << std::endl;
	glfwSetErrorCallback(nullptr);

	// init window
	GLFWwindow *window = glfwCreateWindow(width, height, "Risk", NULL, NULL);
	if (!window) {
		std::cout << "error: window did not init" << std::endl;
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(
			window, []([[maybe_unused]] GLFWwindow *w, int width, int height) {
				glViewport(0, 0, width, height);
			});
	// glfwSetKeyCallback(window, key_callback);
	return window;
}

void Renderer::loadGL() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "error: GLAD did not load GL fns" << std::endl;
		return;
	}
	gladLoadGL();
	// glViewport(0, 0, width, height); // only necessary if we don't want the
	// whole window. This puts it in the wrong pos for some reason
}
