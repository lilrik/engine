#include "renderer.hpp"
#include "shader.hpp"
#include "window.hpp"
#include <cmath>
#include <iostream>
// #include <ranges> eventually figure out how to make this work
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace render;

glm::mat4 makeTransMat() {
	// glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
	//// identity
	// glm::mat4 trans = glm::mat4(1.0f);
	//// get transformation matrix by adding a translation vector to identity matrix
	// trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
	// vec = trans * vec;
	// std::cout << vec.x << vec.y << vec.z << std::endl;

	// scale and rotate container
	// attention: glm expects radians
	// attention: glm expects a unit vector as the rotation axis
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
	return trans;
}

glm::mat4 makeRotMat() {
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
	trans =
			glm::rotate(trans, (float)glfwGetTime() /* angle over time*/, glm::vec3(0.0f, 0.0f, 1.0f));
	return trans;
}

void mats3D(const Shader *program, const Window *window) {
	//// orthographic projection
	//// left, right, bottom, top, near, far
	// glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);

	// fov (realistic = 45), aspect ratio, near, far
	// glm::mat4 proj =
	//		glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	auto modelM = glm::mat4(1.0f);
	// rotate along x to lay obj on floor
	// modelM = glm::rotate(modelM, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelM =
			glm::rotate(modelM, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
	program->uniformM("model", glUniformMatrix4fv, 1, false, glm::value_ptr(modelM));

	auto viewM = glm::mat4(1.0f);
	// move scene forward (neg z) in order to move camera backwards
	viewM = glm::translate(viewM, glm::vec3(0.0f, 0.0f, -3.0f));
	program->uniformM("view", glUniformMatrix4fv, 1, false, glm::value_ptr(viewM));

	auto projectionM = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	program->uniformM("projection", glUniformMatrix4fv, 1, false, glm::value_ptr(projectionM));
}

void uniforms(Shader *program) {
	const auto time_value = glfwGetTime(), greenValue = (sin(time_value) / 2.0f) + 0.5f,
						 redValue = (cos(time_value) / 2.0f) + 0.5f;
	program->uniform("ourColor", glUniform4f, redValue, greenValue, 0.0f, 1.0f);
}

void elementBuffer() {
	const unsigned indices[] = {
			0, 1,
			3, // t1
			3, 2,
			1 // t2
	};

	// gen ele buf obj
	unsigned ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void vertexBuffer() {
	// clang-format off
	//const float verts[] = {
  //  // positions          // colors           // texture coords
  //   0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
  //   0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
  //  -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
  //  -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	//};
		const float verts[] = {
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	};
	// clang-format on

	// gen vert buf obj
	unsigned vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// copy data
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
}

void setupBuffers() {
	// gen vert arr obj
	// essentially a wrapper around the vert attrib pointer configs and ebo so we
	// do not have to repeat the code or rebind every part manually. It's an array
	// of attributes (and indices if ebo defined).
	unsigned vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// elementBuffer();

	vertexBuffer();

	// how to interpret data (this applies to the currently bound buffer)
	// glVertexAttribPointer(0 /*which attribute (the location specified in vert shader)*/,
	//											3 /*how many components in a single attrib ele (how many components it
	//													 reads after doing one stride jump)*/
	//											,
	//											GL_FLOAT /*type of the components*/, GL_FALSE /*normalize*/,
	//											8 * sizeof(float) /*stride (0 in packed structs does it automatically)*/,
	//											(void *)0 /*start offset*/);
	// glEnableVertexAttribArray(0); // ALWAYS ENABLE BECAUSE IT IS NOT BY DEFAULT
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 *
	// sizeof(float))); glEnableVertexAttribArray(1); // ALWAYS ENABLE BECAUSE IT IS NOT BY DEFAULT
	// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 *
	// sizeof(float))); glEnableVertexAttribArray(2); // ALWAYS ENABLE BECAUSE IT IS NOT BY DEFAULT

	glVertexAttribPointer(0 /*which attribute (the location specified in vert shader)*/,
												3 /*how many components in a single attrib ele (how many components it
														 reads after doing one stride jump)*/
												,
												GL_FLOAT /*type of the components*/, GL_FALSE /*normalize*/,
												5 * sizeof(float) /*stride (0 in packed structs does it automatically)*/,
												(void *)0 /*start offset*/);
	glEnableVertexAttribArray(0); // ALWAYS ENABLE BECAUSE IT IS NOT BY DEFAULT
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); // ALWAYS ENABLE BECAUSE IT IS NOT BY DEFAULT
}

void renderCubes(const Shader *program) {
	// auto viewM = glm::mat4(1.0f);
	//// move scene forward (neg z) in order to move camera backwards
	// viewM = glm::translate(viewM, glm::vec3(0.0f, 0.0f, -3.0f));
	// program->uniformM("view", glUniformMatrix4fv, 1, false, glm::value_ptr(viewM));

	auto projectionM = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	program->uniformM("projection", glUniformMatrix4fv, 1, false, glm::value_ptr(projectionM));
	glm::vec3 cubePositions[] = {glm::vec3(0.0f, 0.0f, 0.0f),		 glm::vec3(2.0f, 5.0f, -15.0f),
															 glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
															 glm::vec3(2.4f, -0.4f, -3.5f),	 glm::vec3(-1.7f, 3.0f, -7.5f),
															 glm::vec3(1.3f, -2.0f, -2.5f),	 glm::vec3(1.5f, 2.0f, -2.5f),
															 glm::vec3(1.5f, 0.2f, -1.5f),	 glm::vec3(-1.3f, 1.0f, -1.5f)};
	for (auto i = 0; i < 10; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		float angle = 40.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		program->uniformM("model", glUniformMatrix4fv, 1, false, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

Renderer::Renderer() {
	loadGL();
	program.reset(new Shader());
}

void Renderer::loop() {
	// we have to clear this buffer (like the color buffer) every frame (done below)
	glEnable(GL_DEPTH_TEST);

	setupBuffers();
	program->setupTexture("texture0", "rsc/container.jpg", false);
	program->setupTexture("texture1", "rsc/smiley.png", true);

	// const auto trans_mat = makeTransMat();
	// program->uniformM("transform", glUniformMatrix4fv, 1, false, glm::value_ptr(trans_mat));

	while (!window.shouldClose()) {
		window.handleInput(program.get());

		if (const auto err = glGetError(); err) {
			std::cout << "gl error code " << err << std::endl;
			exit(1);
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// uniforms(program);

		// program->use();
		// const auto rot_mat = makeRotMat();
		// program->uniformM("transform", glUniformMatrix4fv, 1, false, glm::value_ptr(rot_mat));

		//  glBindVertexArray(vao);
		//    bind ebo if needed but usually not because it is also wrapped by the vao

		//  second arg is num of verts!
		//  third arg is index (see explanation) below
		// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// first is mode
		// second is first index in the array we specified in attributes (the offset just says where the
		// array starts in the data and here it actually says which part of the array we want it to be
		// the first)
		// third is num verts
		// mats3D(program.get(), &window);
		// glDrawArrays(GL_TRIANGLES, 0, 36);
		// glBindVertexArray(0);

		renderCubes(program.get());

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
