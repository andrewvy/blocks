#pragma once

#include <GLFW/glfw3.h>
#include "third-party/linmath.h"

typedef struct {
  mat4x4 matrix;

  vec3 position;
  vec3 center;
  vec3 up;

  float horizontalAngle;
  float verticalAngle;
} camera;

void init_camera(camera *cam);
void recalculate_camera(camera *cam);
void move_camera(camera *cam, GLenum key, float deltaTime);
