#pragma once

#include <GLFW/glfw3.h>

typedef struct {
  mat4_t matrix;

  vec3_t center;
  vec3_t up;
} camera;

camera *create_camera();
void destroy_camera(camera *cam);
void recalculate_camera(camera *cam, float verticalAngle, float horizontalAngle, vec3_t position, float speed);
