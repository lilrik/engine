#pragma once
#include "camera.hpp"
#include "common.hpp"
#include "shader.hpp"

namespace render {
struct Data {
	Camera *cam = nullptr;
	Shader *prog = nullptr;
};
struct Window {
	static constexpr auto default_w = 700;
	static constexpr auto default_h = 700;
	Window(int width = default_w, int height = default_h);
	~Window();
	void handleInput(const Shader *p, Camera &cam);
	// inline functions live in hpp
	inline void swapBuffersAndPollEvents() {
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	inline bool shouldClose() { return glfwWindowShouldClose(window); }
	void setupMouse(Camera &cam, Shader *prog);

private:
	Data data;
	GLFWwindow *window;
};
} // namespace render
