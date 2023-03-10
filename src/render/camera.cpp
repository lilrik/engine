#include <render/camera.hpp>

using namespace render;

void Camera::handleMouseInput(double xpos, double ypos) {
  // calculate axis offset compared to last mouse move
  auto xoffset = xpos - prev_pos.mouse_x;
  auto yoffset =
      -1.0f * (ypos - prev_pos.mouse_y); // reversed since y-coordinates range
                                         // from bottom to top
  // update positions history for next frame
  prev_pos.mouse_x = xpos;
  prev_pos.mouse_y = ypos;

  // adjust offset magnitude
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  // add offset to pitch and yaw
  prev_pos.yaw += xoffset;
  prev_pos.pitch += yoffset;

  // constraints (flips at 90 so we use 90 - 1)
  if (prev_pos.pitch > 89.0f)
    prev_pos.pitch = 89.0f;
  if (prev_pos.pitch < -89.0f)
    prev_pos.pitch = -89.0f;

  recalcFrontDir();
  updateLookAt();
}

void Camera::handleScrollInput(double xoffset, double yoffset) {
  fov -= (float)yoffset;
  if (fov < 1.0f)
    fov = 1.0f;
  if (fov > 45.0f)
    fov = 45.0f;
}

void Camera::adjustSpeed() {
  // get time of the frame we're on
  const auto current_frame_time = glfwGetTime();
  // how much time it has been since last frame (how much time it took to
  // execute)
  delta_time = current_frame_time - prev_frame_time;
  // scale speed accordingly
  speed = units_per_s * delta_time;
  // set the new previous frame time
  prev_frame_time = current_frame_time;
}

void Camera::handleKeyInput(const std::array<int, 5> &keys_pressed) {
  auto needs_update = false;
  for (int key : keys_pressed) {
    switch (key) {
    // these movements work by adding (or subtracting aka adding in the
    // opposite direction) to the position of the camera a normalized
    // direction vector
    //
    // we already have the front/back direction and use the cross product of
    // it with the up direction to get a left/right direction
    case GLFW_KEY_W:
      pos += front * speed;
      break;
    case GLFW_KEY_S:
      pos -= front * speed;
      break;
    case GLFW_KEY_A:
      pos -= glm::normalize(glm::cross(front, up)) * speed;
      break;
    case GLFW_KEY_D:
      pos += glm::normalize(glm::cross(front, up)) * speed;
      break;
    case GLFW_KEY_SPACE:
      pos += up * speed;
      break;
    case GLFW_KEY_LEFT_SHIFT:
      pos -= up * speed;
      break;
    default:
      continue;
    }
    needs_update = true;
  }

  if (needs_update)
    updateLookAt();
}

void Camera::updateLookAt() {
  for (auto &program : programs) {
    program.use();
    program.uniformM("view", glUniformMatrix4fv, 1, false,
                     // second argument is the point for the camera to focus on
                     // here we grab the camera position point and "move it
                     // forward" by adding the front direction vector
                     glm::value_ptr(glm::lookAt(pos, pos + front,
                                                glm::vec3(0.0f, 1.0f, 0.0f))));
  }
}

void Camera::updateProjection(float width, float height) {
  for (auto &program : programs) {
    program.use();
    program.updateProj(
        glm::perspective(glm::radians(fov), width / height, 0.1f, 100.0f));
  }
}

inline void Camera::recalcFrontDir() {
  front = glm::normalize(glm::vec3(
      cos(glm::radians(prev_pos.yaw)) * cos(glm::radians(prev_pos.pitch)),
      sin(glm::radians(prev_pos.pitch)),
      sin(glm::radians(prev_pos.yaw)) * cos(glm::radians(prev_pos.pitch))));
}
