#include <GLFW/glfw3.h>

#include "third-party/linmath.h"
#include "camera.h"

void init_camera(camera *cam) {
  cam->position[0] = 3;
  cam->position[1] = 3;
  cam->position[2] = 3;

  cam->center[0] = 3;
  cam->center[1] = 3;
  cam->center[2] = 3;

  cam->up[0] = 0;
  cam->up[1] = 1;
  cam->up[2] = 0;

  cam->horizontalAngle = 3.95f;
  cam->verticalAngle = -0.6f;

  mat4x4_look_at(cam->matrix, cam->position, cam->center, cam->up);
}

void recalculate_camera(camera *cam) {
  vec3 direction = {
    cos(cam->verticalAngle) * sin(cam->horizontalAngle),
    sin(cam->verticalAngle),
    cos(cam->verticalAngle) * cos(cam->horizontalAngle)
  };

  vec3 right = {
    sin(cam->horizontalAngle - 3.14f/2.0f),
    0,
    cos(cam->horizontalAngle - 3.14f/2.0f)
  };

  vec3 up;
  vec3_mul_cross(up, right, direction);

  vec3 position_direction;
  vec3_add(position_direction, cam->position, direction);

  mat4x4_look_at(cam->matrix, cam->position, position_direction, up);
}

static const float speed = 3.0f;

void move_camera(camera *cam, GLenum key, float deltaTime) {
  vec3 direction = {
    cos(cam->verticalAngle) * sin(cam->horizontalAngle),
    sin(cam->verticalAngle),
    cos(cam->verticalAngle) * cos(cam->horizontalAngle)
  };

  vec3 right = {
    sin(cam->horizontalAngle - 3.14f/2.0f),
    0,
    cos(cam->horizontalAngle - 3.14f/2.0f)
  };

  vec3 scaled_direction;
  vec3_scale(scaled_direction, direction, deltaTime);
  vec3_scale(scaled_direction, scaled_direction, speed);

  vec3 scaled_right;
  vec3_scale(scaled_right, right, deltaTime);
  vec3_scale(scaled_right, scaled_right, speed);

  switch (key) {
    case GLFW_KEY_A:
      vec3_sub(cam->position, cam->position, scaled_right);
      break;
    case GLFW_KEY_D:
      vec3_add(cam->position, cam->position, scaled_right);
      break;
    case GLFW_KEY_W:
      vec3_add(cam->position, cam->position, scaled_direction);
      break;
    case GLFW_KEY_S:
      vec3_sub(cam->position, cam->position, scaled_direction);
      break;
  }

}
