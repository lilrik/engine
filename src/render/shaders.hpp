#pragma once

static constexpr auto num_programs = 2;

static constexpr auto main_vert = R"(
		#version 330 core
		layout (location = 0) in vec3 aPos;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		void main() {
			gl_Position = projection * view * model * vec4(aPos, 1.0);
		}
	)";

static constexpr auto main_frag = R"(
		#version 330 core
		out vec4 FragColor;

		// the amount of each color component it reflects from a light source
		uniform vec3 objectColor;
		uniform vec3 lightColor;

		void main() {
			FragColor = vec4(lightColor * objectColor, 1.0);
		} 
	)";

static constexpr auto light_vert = R"(
		#version 330 core
		layout (location = 0) in vec3 aPos;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		void main() {
			gl_Position = projection * view * model * vec4(aPos, 1.0);
		}
	)";

static constexpr auto light_frag = R"(
		#version 330 core
		out vec4 FragColor;

		void main() {
			FragColor = vec4(1.0);
		} 
	)";
