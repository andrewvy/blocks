#pragma once

#include <GLFW/glfw3.h>
#include "common.h"

camera *create_camera();
void destroy_camera(camera *cam);
void recalculate_camera(camera *cam, float verticalAngle, float horizontalAngle, vec3_t position, float speed);
