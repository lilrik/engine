#include "shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace render {
struct Camera {
	glm::vec3 pos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	const float speed = 0.05f;
	inline void updateLookAt(const Shader *p) {
		p->uniformM("view", glUniformMatrix4fv, 1, false,
								// think of the middle arg of lookAt as the target direction. We sum the position to
								// keep it looking in the same direction
								glm::value_ptr(glm::lookAt(pos, pos + front, glm::vec3(0.0f, 1.0f, 0.0f))));
	}
};
} // namespace render
