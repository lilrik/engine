#pragma once

static constexpr auto num_programs = 2;

static constexpr auto main_vert = R"(
		#version 330 core
		layout (location = 0) in vec3 position;
		layout (location = 1) in vec3 normal;
		layout (location = 2) in vec2 texture_coord;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		out vec3 frag_pos;
		out vec3 frag_normal;
		out vec2 frag_texture_coord;

		void main() {
			// all the calculations in this program was made in world coords (for learning purposes) but they are usually done in view coords

			frag_pos = vec3(model * vec4(position, 1.0));
			// normal matrix in world space: cast to mat3 to lose translation and to be able to mult by vec3
			// now the normal vec is in world space
			// note: inversing mats is costly and we should do all this in the cpu and send it via uniform
			// this normal mat is only really needed when there is non uniform scaling (but we always need to remove the translation part by casting to mat3 or setting w = 0)
			frag_normal = mat3(transpose(inverse(model))) * normal; 
			frag_texture_coord = texture_coord;

			gl_Position = projection * view * model * vec4(position, 1.0);
		}
	)";

static constexpr auto main_frag = R"(
		#version 330 core

		#define NR_POINT_LIGHTS 4  

		out vec4 frag_color;

		// the amount of each color component it reflects from a light source
		// sampler2D is an opaque type so it must be instantiated as a uniform
		struct Material {
			sampler2D diffuse;
			sampler2D specular;
			float shininess;
		};

		struct Phong {
			vec3 ambient;
			vec3 diffuse;
			vec3 specular;
		};

		struct Attenuation {
			float constant;
			float linear;
			float quadratic;  
		};

		struct Spot {
			vec3 front_dir;
			float cut_off;
			float outer_cut_off;
		};

		struct DirLight {
			vec3 direction;
			Phong phong_props;
		};  

		struct PointLight {    
			vec3 position;
			Phong phong_props;
			Attenuation attenuation_props;
		};  

		struct Spotlight {
			vec3 position;
			Spot spot_props;
			Phong phong_props;
			Attenuation attenuation_props;
		};  

		uniform DirLight dir_light;
		uniform PointLight point_lights[NR_POINT_LIGHTS];
		uniform Spotlight spotlight;
		uniform Material material;
		uniform vec3 view_pos;

		in vec3 frag_pos;
		in vec3 frag_normal;
		in vec2 frag_texture_coord;

		vec3 calc_dir_light(DirLight light, vec3 normal, vec3 frag_to_view_dir);
		vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 frag_to_view_dir);
		vec3 calc_spotlight(Spotlight light, vec3 normal, vec3 frag_pos, vec3 frag_to_view_dir);
		float calc_diffuse(vec3 normal, vec3 frag_to_light_dir);
		float calc_specular(vec3 normal, vec3 frag_to_light_dir, vec3 frag_to_view_dir, Material material);
		float calc_attenuation(vec3 light_pos, Attenuation light, vec3 frag_pos);
		float calc_spotlight_intensity(vec3 frag_to_light_dir, Spot light);
		vec3 calc_phong_res(Phong light, Material material, vec2 frag_texture_coord, float diff, float spec);

		void main() {
			// properties
			vec3 norm = normalize(frag_normal);
			vec3 frag_to_view_dir = normalize(view_pos - frag_pos);

			vec3 result = calc_dir_light(dir_light, norm, frag_to_view_dir);

			for(int i = 0; i < NR_POINT_LIGHTS; i++)
				result += calc_point_light(point_lights[i], norm, frag_pos, frag_to_view_dir);    

			result += calc_spotlight(spotlight, norm, frag_pos, frag_to_view_dir);    
			
			frag_color = vec4(result, 1.0);
		} 

		vec3 calc_dir_light(DirLight light, vec3 normal, vec3 frag_to_view_dir) {
				vec3 frag_to_light_dir = normalize(-light.direction); // because we want the dir from frag to light and dir lights specify a dir from light to ground

				float diff = calc_diffuse(normal, frag_to_light_dir);
				float spec = calc_specular(normal, frag_to_light_dir, frag_to_view_dir, material);

				return calc_phong_res(light.phong_props, material, frag_texture_coord, diff, spec);
		}

	vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 frag_to_view_dir) {
			vec3 frag_to_light_dir = normalize(light.position - frag_pos);

			float diff = calc_diffuse(normal, frag_to_light_dir);
			float spec = calc_specular(normal, frag_to_light_dir, frag_to_view_dir, material);
			float attenuation = calc_attenuation(light.position, light.attenuation_props, frag_pos);

			return calc_phong_res(light.phong_props, material, frag_texture_coord, diff, spec) * attenuation;
	}

	vec3 calc_spotlight(Spotlight light, vec3 normal, vec3 frag_pos, vec3 frag_to_view_dir) {
			vec3 frag_to_light_dir = normalize(light.position - frag_pos);

			float diff = calc_diffuse(normal, frag_to_light_dir);
			float spec = calc_specular(normal, frag_to_light_dir, frag_to_view_dir, material);
			float attenuation = calc_attenuation(light.position, light.attenuation_props, frag_pos);
			float intensity = calc_spotlight_intensity(frag_to_light_dir, light.spot_props);

			return calc_phong_res(light.phong_props, material, frag_texture_coord, diff, spec) * intensity * attenuation;
	}

	float calc_diffuse(vec3 normal, vec3 frag_to_light_dir) {return max(dot(normal, frag_to_light_dir), 0.0);}

	float calc_specular(vec3 normal, vec3 frag_to_light_dir, vec3 frag_to_view_dir, Material material) {
			vec3 reflection_dir = reflect(-frag_to_light_dir, normal);
			return pow(max(dot(frag_to_view_dir, reflection_dir), 0.0), material.shininess);
	}

	float calc_attenuation(vec3 light_pos, Attenuation light, vec3 frag_pos) {
			float distance    = length(light_pos - frag_pos);
			return 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
	}

	float calc_spotlight_intensity(vec3 frag_to_light_dir, Spot light) {
			float theta = dot(frag_to_light_dir, normalize(-light.front_dir));
			float epsilon   = light.cut_off - light.outer_cut_off;
			return clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);
	}

	vec3 calc_phong_res(Phong light, Material material, vec2 frag_texture_coord, float diff, float spec) {
			vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, frag_texture_coord));
			vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, frag_texture_coord));
			vec3 specular = light.specular * spec * vec3(texture(material.specular, frag_texture_coord));

			return ambient + diffuse + specular;
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
		out vec4 frag_color;

		void main() {
			frag_color = vec4(1.0);
		} 
	)";
