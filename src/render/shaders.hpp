#pragma once

static constexpr auto num_programs = 2;

static constexpr auto main_vert = R"(
		#version 330 core
		layout (location = 0) in vec3 a_position;
		layout (location = 1) in vec3 a_normal;
		layout (location = 2) in vec2 a_text_coord;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		out vec3 FragPos;
		out vec3 Normal;
		out vec2 TexCoo;

		void main() {
			// all the calculations in this program was made in world coords (for learning purposes) but they are usually done in view coords

			FragPos = vec3(model * vec4(a_position, 1.0));
			// normal matrix in world space: cast to mat3 to lose translation and to be able to mult by vec3
			// now the normal vec is in world space
			// note: inversing mats is costly and we should do all this in the cpu and send it via uniform
			// this normal mat is only really needed when there is non uniform scaling (but we always need to remove the translation part by casting to mat3 or setting w = 0)
			Normal = mat3(transpose(inverse(model))) * a_normal; 
			TexCoo = a_text_coord;

			gl_Position = projection * view * model * vec4(a_position, 1.0);
		}
	)";

static constexpr auto main_frag = R"(
		#version 330 core

		#define NR_POINT_LIGHTS 4  

		out vec4 FragColor;

		// the amount of each color component it reflects from a light source
		// sampler2D is an opaque type so it must be instantiated as a uniform
		struct Material {
			sampler2D diffuse;
			sampler2D specular;
			float shininess;
		};

		struct DirLight {
			vec3 direction;
		
			vec3 ambient;
			vec3 diffuse;
			vec3 specular;
		};  

		struct PointLight {    
			vec3 position;
			
			float constant;
			float linear;
			float quadratic;  

			vec3 ambient;
			vec3 diffuse;
			vec3 specular;
		};  

		struct Spotlight {
			vec3 frontDir;

			float cutOff;
			float outerCutOff;

			float constant;
			float linear;
			float quadratic;  
		
			vec3 ambient;
			vec3 diffuse;
			vec3 specular;
		};  

		uniform DirLight dirLight;
		uniform PointLight pointLights[NR_POINT_LIGHTS];
		uniform Spotlight spotlight;
		uniform Material material;
		uniform vec3 viewPos;

		in vec3 FragPos;
		in vec3 Normal;
		in vec2 TexCoo;

		vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
		vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
		vec3 CalcSpotlight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir);

		void main() {
			// properties
			vec3 norm = normalize(Normal);
			vec3 viewDir = normalize(viewPos - FragPos);

			// phase 1: Directional lighting
			vec3 result =  0 * CalcDirLight(dirLight, norm, viewDir);
			// phase 2: Point lights
			for(int i = 0; i < NR_POINT_LIGHTS; i++)
				result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
			// phase 3: Spot light
			result += CalcSpotlight(spotlight, norm, FragPos, viewDir);    
			
			FragColor = vec4(result, 1.0);
		} 

		vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
				vec3 lightDir = normalize(-light.direction); // because we want the dir from frag to light and dir lights specify a dir from light to ground
				// diffuse shading
				float diff = max(dot(normal, lightDir), 0.0);
				// specular shading
				vec3 reflectDir = reflect(-lightDir, normal);
				float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
				// combine results
				vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoo));
				vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoo));
				vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoo));
				return (ambient + diffuse + specular);
		}

	vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
			vec3 lightDir = normalize(light.position - fragPos);
			// diffuse shading
			float diff = max(dot(normal, lightDir), 0.0);
			// specular shading
			vec3 reflectDir = reflect(-lightDir, normal);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
			// attenuation
			float distance    = length(light.position - fragPos);
			float attenuation = 1.0 / (light.constant + light.linear * distance + 
							 light.quadratic * (distance * distance));    
			// combine results
			vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoo));
			vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoo));
			vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoo));
			return (ambient + diffuse + specular) * attenuation;
	}

	vec3 CalcSpotlight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
			vec3 lightDir = normalize(viewPos - fragPos);
			// diffuse shading
			float diff = max(dot(normal, lightDir), 0.0);
			// specular shading
			vec3 reflectDir = reflect(-lightDir, normal);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
			// attenuation
			float distance    = length(viewPos - fragPos);
			float attenuation = 1.0 / (light.constant + light.linear * distance + 
							 light.quadratic * (distance * distance));    
			// spotlight cone
			float theta = dot(lightDir, normalize(-light.frontDir));
			float epsilon   = light.cutOff - light.outerCutOff;
			float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
			// combine results
			vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoo));
			vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoo));
			vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoo));
			return (ambient + diffuse + specular) * intensity * attenuation;
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
