#include <GLFW/glfw3.h>

#include "third-party/math_3d.h"
#include "camera.h"

void init_camera(camera *cam) {
  cam->position.x = -5;
  cam->position.y = 260;
  cam->position.z = -5;
  cam->speed = 10.0f;

  cam->center.x = -1;
  cam->center.y = 0;
  cam->center.z = -1;

  cam->up.x = 0;
  cam->up.y = 1;
  cam->up.z = 0;

  cam->horizontalAngle = 0.60f;
  cam->verticalAngle = -0.15f;

  cam->matrix = m4_look_at(cam->position, cam->center, cam->up);
}

void recalculate_camera(camera *cam) {
  vec3_t direction = {
    cos(cam->verticalAngle) * sin(cam->horizontalAngle),
    sin(cam->verticalAngle),
    cos(cam->verticalAngle) * cos(cam->horizontalAngle)
  };

  vec3_t right = {
    sin(cam->horizontalAngle - 3.14f/2.0f),
    0,
    cos(cam->horizontalAngle - 3.14f/2.0f)
  };

  vec3_t up = v3_cross(right, direction);
  vec3_t position_direction = v3_add(cam->position, direction);

  cam->matrix = m4_look_at(cam->position, position_direction, up);
}

void move_camera(camera *cam, GLenum key, float deltaTime) {
  vec3_t direction = vec3(
    cos(cam->verticalAngle) * sin(cam->horizontalAngle),
    sin(cam->verticalAngle),
    cos(cam->verticalAngle) * cos(cam->horizontalAngle)
  );

  vec3_t right = vec3(
    sin(cam->horizontalAngle - 3.14f/2.0f),
    0,
    cos(cam->horizontalAngle - 3.14f/2.0f)
  );

  vec3_t scaled_direction = v3_muls(direction, deltaTime);
  scaled_direction = v3_muls(scaled_direction, cam->speed);

  vec3_t scaled_right = v3_muls(right, deltaTime);
  scaled_right = v3_muls(scaled_right, cam->speed);

  switch (key) {
    case GLFW_KEY_A:
      cam->position = v3_sub(cam->position, scaled_right);
      break;
    case GLFW_KEY_D:
      cam->position = v3_add(cam->position, scaled_right);
      break;
    case GLFW_KEY_W:
      cam->position = v3_add(cam->position, scaled_direction);
      break;
    case GLFW_KEY_S:
      cam->position = v3_sub(cam->position, scaled_direction);
      break;
  }

}
