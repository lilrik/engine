#include "renderer.hpp"
#include "shader.hpp"
#include <cmath>
#include <iostream>
#include <memory>
// #include <ranges> eventually figure out how to make this work

using namespace render;

void uniforms(unsigned program) {
	const auto time_value = glfwGetTime(),
						 greenValue = (sin(time_value) / 2.0f) + 0.5f,
						 redValue = (cos(time_value) / 2.0f) + 0.5f;
	const auto vertexColorLocation =
			glGetUniformLocation(program, "ourColor"); // -1 == not found
	// glUseProgram(program); this is already set before running this function
	glUniform4f(vertexColorLocation, redValue, greenValue, 0.0f, 1.0f);
}

unsigned buffers() {
	const float verts[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bl
			0.5f,	 -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // br
			-0.5f, 0.5f,	0.0f, 1.0f, 1.0f, 1.0f, // tl
			0.5f,	 0.5f,	0.0f, 0.0f, 0.0f, 1.0f, // tr
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
	// of attributes (and indices if ebo defined).
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
			3 /*how many components in a single attrib ele (how many components it
					 reads after doing one stride jump)*/
			,
			GL_FLOAT /*type of the components*/, GL_FALSE /*normalize*/,
			6 * sizeof(float) /*stride (0 in packed structs does it automatically)*/,
			(void *)0 /*start offset*/);
	glEnableVertexAttribArray(0); // ALWAYS ENABLE BECAUSE IT IS NOT BY DEFAULT
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
												(void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); // ALWAYS ENABLE BECAUSE IT IS NOT BY DEFAULT

	return vao;
}

Renderer::Renderer() {
	window = initGLFW();
	loadGL();
	program = std::unique_ptr<Shader>(new Shader());
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

		// uniforms(program);

		glUseProgram(program->id);
		const auto vao = buffers();
		glBindVertexArray(vao);
		//  bind ebo if needed but usually not because it is also wrapped by the vao
		//  second arg is num of verts!
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// glBindVertexArray(0);

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
