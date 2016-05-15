#include <stdlib.h>
#include <GLFW/glfw3.h>

#include "third-party/math_3d.h"
#include "camera.h"

camera *create_camera() {
  camera *cam = malloc(sizeof(camera));

  cam->center.x = -1;
  cam->center.y = 0;
  cam->center.z = -1;

  cam->up.x = 0;
  cam->up.y = 1;
  cam->up.z = 0;

  cam->matrix = m4_look_at(vec3(0, 0, 0), cam->center, cam->up);

  return cam;
}

void destroy_camera(camera *cam) {
  free(cam);
}

void recalculate_camera(camera *cam, float verticalAngle, float horizontalAngle, vec3_t position, float speed) {
  vec3_t direction = {
    cos(verticalAngle) * sin(horizontalAngle),
    sin(verticalAngle),
    cos(verticalAngle) * cos(horizontalAngle)
  };

  vec3_t right = {
    sin(horizontalAngle - 3.14f/2.0f),
    0,
    cos(horizontalAngle - 3.14f/2.0f)
  };

  vec3_t up = v3_cross(right, direction);
  vec3_t position_direction = v3_add(position, direction);

  cam->matrix = m4_look_at(position, position_direction, up);
}
