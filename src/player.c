#include <stdlib.h>
#include <GLFW/glfw3.h>

#include "third-party/math_3d.h"
#include "player.h"
#include "camera.h"

Player *create_player() {
  Player *player = malloc(sizeof(Player));
  player->cam = *create_camera();

  player->speed = 10.0f;
  player->horizontalAngle = 0.60f;
  player->verticalAngle = -0.15f;

  player->position.x = -5;
  player->position.y = 260;
  player->position.z = -5;

  return player;
}

void destroy_player(Player *player) {
  destroy_camera(&(player->cam));
  free(player);
}

static float oldVerticalAngle = 0.0;
static float oldHorizontalAngle = 0.0;
static vec3_t oldPosition;
static float oldSpeed = 0.0;

void recalculate_player(Player *player) {
  // Only recalculate if values have changed.
  if (oldVerticalAngle != player->verticalAngle ||
      oldHorizontalAngle != player->horizontalAngle ||
      oldPosition.x != player->position.x ||
      oldPosition.y != player->position.y ||
      oldPosition.z != player->position.z ||
      oldSpeed != player->speed) {

    recalculate_camera(
      &(player->cam),
      player->verticalAngle,
      player->horizontalAngle,
      player->position,
      player->speed
    );
  }
}

void move_player(Player *player, GLenum key, float deltaTime) {
  vec3_t direction = vec3(
    cos(player->verticalAngle) * sin(player->horizontalAngle),
    sin(player->verticalAngle),
    cos(player->verticalAngle) * cos(player->horizontalAngle)
  );

  vec3_t right = vec3(
    sin(player->horizontalAngle - 3.14f/2.0f),
    0,
    cos(player->horizontalAngle - 3.14f/2.0f)
  );

  vec3_t scaled_direction = v3_muls(direction, deltaTime);
  scaled_direction = v3_muls(scaled_direction, player->speed);

  vec3_t scaled_right = v3_muls(right, deltaTime);
  scaled_right = v3_muls(scaled_right, player->speed);

  switch (key) {
    case GLFW_KEY_A:
      player->position = v3_sub(player->position, scaled_right);
      break;
    case GLFW_KEY_D:
      player->position = v3_add(player->position, scaled_right);
      break;
    case GLFW_KEY_W:
      player->position = v3_add(player->position, scaled_direction);
      break;
    case GLFW_KEY_S:
      player->position = v3_sub(player->position, scaled_direction);
      break;
  }
}
