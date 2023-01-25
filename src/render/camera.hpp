#pragma once

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <render/program.hpp>
#include <render/shaders.hpp>

namespace render {
struct Position {
  float mouse_x = 0, mouse_y = 0, pitch = 0 /*plane*/,
        yaw = -90 /*yeeehaw TODO: why is -90 right*/;
};
// how does this shit not give a stack buffer overflow anymore
struct Camera {
  static constexpr auto units_per_s = 3.0f;
  static constexpr auto sensitivity = 0.3f;
  float delta_time, // time between current frame and last frame (aka duration
                    // of execution of curr frame)
      prev_frame_time, // time of last frame (aka NOT duration but the actual
                       // point in time)
      speed;
  Position prev_pos;
  float fov;
  glm::vec3 pos, front, up;
  // programs which depend on lookAt
  std::array<Program, num_programs> &programs;

  Camera(std::array<Program, 2> &programs)
      : delta_time(0.0f), prev_frame_time(0.0f), speed(0.0f), fov(45.0f),
        pos(glm::vec3(0.0f, 0.0f, 3.0f)), front(glm::vec3(0.0f, 0.0f, -1.0f)),
        up(glm::vec3(0.0f, 1.0f, 0.0f)), programs(programs){};

  void adjustSpeed();
  void handleKeyInput(const std::array<int, 5> &keys_pressed);
  void handleMouseInput(double xpos, double ypos);
  void handleScrollInput(double xoffset, double yoffset);

  // WARNING: we use program.use() here
  void updateLookAt();

private:
  void recalcFrontDir();
};
} // namespace render
