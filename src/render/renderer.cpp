#include "renderer.hpp"
#include "shader.hpp"
#include "window.hpp"
#include <cmath>
#include <iostream>
// #include <ranges> eventually figure out how to make this work

using namespace render;

void uniforms(Shader *program) {
	const auto time_value = glfwGetTime(), greenValue = (sin(time_value) / 2.0f) + 0.5f,
						 redValue = (cos(time_value) / 2.0f) + 0.5f;
	program->uniform("ourColor", glUniform4f, redValue, greenValue, 0.0f, 1.0f);
}

unsigned buffers() {
	const float verts[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bl
			0.5f,	 -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // br
			-0.5f, 0.5f,	0.0f, 1.0f, 1.0f, 1.0f, // tl
			0.5f,	 0.5f,	0.0f, 0.0f, 0.0f, 1.0f, // tr
	};

	const unsigned indices[] = {
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// gen vert buf obj
	unsigned vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// copy data
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	// how to interpret data (this applies to the currently bound buffer)
	glVertexAttribPointer(0 /*which attribute (the location specified in vert shader)*/,
												3 /*how many components in a single attrib ele (how many components it
														 reads after doing one stride jump)*/
												,
												GL_FLOAT /*type of the components*/, GL_FALSE /*normalize*/,
												6 * sizeof(float) /*stride (0 in packed structs does it automatically)*/,
												(void *)0 /*start offset*/);
	glEnableVertexAttribArray(0); // ALWAYS ENABLE BECAUSE IT IS NOT BY DEFAULT
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); // ALWAYS ENABLE BECAUSE IT IS NOT BY DEFAULT

	return vao;
}

Renderer::Renderer() {
	loadGL();
	program.reset(new Shader());
}

void Renderer::loop() {
	while (!window.shouldClose()) {
		window.handleInput();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// uniforms(program);

		program->use();
		const auto vao = buffers();
		glBindVertexArray(vao);
		//  bind ebo if needed but usually not because it is also wrapped by the vao
		//  second arg is num of verts!
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// glBindVertexArray(0);

		window.swapBuffersAndPollEvents();
	}
}

void Renderer::loadGL() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "error: GLAD did not load GL fns" << std::endl;
		exit(1);
	}
	gladLoadGL();
	// glViewport(0, 0, width, height); // only necessary if we don't want the
	// whole window. This puts it in the wrong pos for some reason
}
