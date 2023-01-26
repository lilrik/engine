#pragma once

static constexpr auto num_programs = 2;

static constexpr auto main_vert = R"(
		#version 330 core
		layout (location = 0) in vec3 aPos;
		layout (location = 1) in vec3 aNormal;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		out vec3 Normal;
		out vec3 FragPos;

		void main() {
			// all the calculations in this program was made in world coords (for learning purposes) but they are usually done in view coords

			// normal matrix in world space: cast to mat3 to lose translation and to be able to mult by vec3
			// now the normal vec is in world space
			// note: inversing mats is costly and we should do all this in the cpu and send it via uniform
			// this normal mat is only really needed when there is non uniform scaling?
			Normal = mat3(transpose(inverse(model))) * aNormal; 
			FragPos = vec3(model * vec4(aPos, 1.0));

			gl_Position = projection * view * model * vec4(aPos, 1.0);
		}
	)";

static constexpr auto main_frag = R"(
		#version 330 core
		out vec4 FragColor;

		// the amount of each color component it reflects from a light source
		uniform vec3 objectColor;
		uniform vec3 lightColor;
		uniform vec3 lightPos;
		uniform vec3 viewPos;

		in vec3 Normal;
		in vec3 FragPos;

		void main() {
			// helper vars for diffused and specular
			vec3 normal = normalize(Normal);
			vec3 lightDir = normalize(lightPos - FragPos);
			float specularStrength = 0.5;

			// diffused light
			float diff = max(dot(normal, lightDir), 0.0);
			vec3 diffuse = diff * lightColor;

			// specular light
			vec3 viewDir = normalize(viewPos - FragPos);
			// reflects expects the dir to be from light to point
			vec3 reflectDir = reflect(-lightDir, Normal);
			float shininess = 64;
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
			vec3 specular = specularStrength * spec * lightColor;

			// ambient light
			float ambientStrength = 0.1;
			vec3 ambient = ambientStrength * lightColor;

			FragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
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
