#pragma once

#include <GLFW/glfw3.h>

typedef struct {
  mat4_t matrix;

  vec3_t position;
  vec3_t center;
  vec3_t up;

  float horizontalAngle;
  float verticalAngle;
  float speed;
} camera;

void init_camera(camera *cam);
void recalculate_camera(camera *cam);
void move_camera(camera *cam, GLenum key, float deltaTime);
