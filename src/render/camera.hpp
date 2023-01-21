#pragma once

#include "shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace render {
struct Position {
	float mouse_x = 0, mouse_y = 0, pitch = 0 /*plane*/, yaw = -90 /*yeeehaw TODO: why is -90 right*/;
};
// how does this shit not give a stack buffer overflow anymore
struct Camera {
	static constexpr auto units_per_s = 3.0f;
	static constexpr auto sensitivity = 0.3f;
	float delta_time, // time between current frame and last frame (aka duration of execution of curr
										// frame)
			prev_frame_time, // time of last frame (aka NOT duration but the actual point in time)
			speed;
	Position prev_pos;
	float fov;
	glm::vec3 pos, front, up;

	Camera()
			: delta_time(0.0f), prev_frame_time(0.0f), speed(0.0f), fov(45.0f),
				pos(glm::vec3(0.0f, 0.0f, 3.0f)), front(glm::vec3(0.0f, 0.0f, -1.0f)),
				up(glm::vec3(0.0f, 1.0f, 0.0f)){};

	inline void updateLookAt(const Shader *p) {
		p->uniformM("view", glUniformMatrix4fv, 1, false,
								// second argument is the point for the camera to focus on
								// here we grab the camera position point and "move it forward" by adding the front
								// direction vector (imagine in 3d space)
								glm::value_ptr(glm::lookAt(pos, pos + front, glm::vec3(0.0f, 1.0f, 0.0f))));
	}

	inline void recalcFrontDir() {
		front = glm::normalize(
				glm::vec3(cos(glm::radians(prev_pos.yaw)) * cos(glm::radians(prev_pos.pitch)),
									sin(glm::radians(prev_pos.pitch)),
									sin(glm::radians(prev_pos.yaw)) * cos(glm::radians(prev_pos.pitch))));
	}

	void adjustSpeed() {
		// get time of the frame we're on
		const auto current_frame_time = glfwGetTime();
		// how much time it has been since last frame (how much time it took to execute)
		delta_time = current_frame_time - prev_frame_time;
		// scale speed accordingly
		speed = units_per_s * delta_time;
		// set the new previous frame time
		prev_frame_time = current_frame_time;
	}
};
} // namespace render
