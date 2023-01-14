#include "../lib/glad/include/glad/glad.h"
#include "../lib/glfw/include/GLFW/glfw3.h"
#include "render/renderer.hpp"

int main() {
	render::Renderer r;
	r.loop();
}
