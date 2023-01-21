#include "window.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace render;

Window::Window(int width, int height) {
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
		exit(1);
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, []([[maybe_unused]] GLFWwindow *w, int width, int height) {
		glViewport(0, 0, width, height);
	});
	// glfwSetKeyCallback(window, key_callback);
	this->window = window;
}

Window::~Window() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::handleInput(const Shader *p, Camera &cam) {
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// these movements work by adding (or subtracting aka adding in the opposite direction) to the
	// position of the camera a normalized direction vector
	//
	// we already have the front/back direction and use the cross product of it with the up direction
	// to get a left/right direction
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.pos += cam.speed * cam.front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.pos -= cam.speed * cam.front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.pos -= glm::normalize(glm::cross(cam.front, cam.up)) * cam.speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.pos += glm::normalize(glm::cross(cam.front, cam.up)) * cam.speed;

	cam.updateLookAt(p);
}

void Window::setupMouse(Camera &cam, Shader *prog) {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// associate camera pointer and shader pointer with the window pointer so we can use it inside
	// callback funcs
	data.cam = &cam;
	data.prog = prog;
	glfwSetWindowUserPointer(window, &data);
	glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
		// do not move the camera when the window is not fully focused (mouse hidden)
		if (!glfwGetWindowAttrib(window, GLFW_FOCUSED))
			return;

		// get cam pointer associated with this window
		auto user_data = (Data *)glfwGetWindowUserPointer(window);
		auto cam = user_data->cam;
		auto prog = user_data->prog;

		// calculate axis offset compared to last mouse move
		auto xoffset = xpos - cam->prev_pos.mouse_x;
		auto yoffset =
				-1.0f *
				(ypos - cam->prev_pos.mouse_y); // reversed since y-coordinates range from bottom to top
		// update positions history for next frame
		cam->prev_pos.mouse_x = xpos;
		cam->prev_pos.mouse_y = ypos;

		// adjust offset magnitude
		xoffset *= cam->sensitivity;
		yoffset *= cam->sensitivity;

		// add offset to pitch and yaw
		cam->prev_pos.yaw += xoffset;
		cam->prev_pos.pitch += yoffset;

		// constraints (flips at 90 so we use 90 - 1)
		if (cam->prev_pos.pitch > 89.0f)
			cam->prev_pos.pitch = 89.0f;
		if (cam->prev_pos.pitch < -89.0f)
			cam->prev_pos.pitch = -89.0f;

		cam->recalcFrontDir();
		cam->updateLookAt(prog);
	});

	glfwSetWindowFocusCallback(window, [](GLFWwindow *window, int focused) {
		auto user_data = (Data *)glfwGetWindowUserPointer(window);
		auto cam = user_data->cam;
		if (focused)
			glfwSetCursorPos(window, cam->prev_pos.mouse_x, cam->prev_pos.mouse_y);

		// TODO:
		// onde e que a view esta a ser set pela primeira vez (SPOILER JA VI lookAt())
	});

	glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset) {
		auto user_data = (Data *)glfwGetWindowUserPointer(window);
		auto cam = user_data->cam;

		cam->fov -= (float)yoffset;
		if (cam->fov < 1.0f)
			cam->fov = 1.0f;
		if (cam->fov > 45.0f)
			cam->fov = 45.0f;
	});
}
