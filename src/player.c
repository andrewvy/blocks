#include <stdlib.h>
#include <math.h>
#include <GLFW/glfw3.h>

#include "third-party/math_3d.h"
#include "player.h"
#include "camera.h"
#include "chunk.h"

Player *create_player() {
  Player *player = malloc(sizeof(Player));
  player->state = PLAYER_AIRBORNE;

  player->cam = *create_camera();

  player->speed = 50.0f;
  player->jumpHeight = 5.0f;

  player->horizontalAngle = 0.60f;
  player->verticalAngle = -0.15f;

  player->position.x = 7;
  player->position.y = 260;
  player->position.z = 7;

  player->velocity.x = 0.0;
  player->velocity.y = 0.0;
  player->velocity.z = 0.0;

  player->chunk_position.x = 0;
  player->chunk_position.y = 0;
  player->chunk_position.z = 0;

  player->voxel_position.x = 0;
  player->voxel_position.y = 257;
  player->voxel_position.z = 0;

  return player;
}

void destroy_player(Player *player) {
  destroy_camera(&(player->cam));
  free(player);
}

void recalculate_chunk_position(Player *player) {
  player->chunk_position.x = floor(player->position.x / CHUNK_X);
  player->chunk_position.y = 0.0;
  player->chunk_position.z = floor(player->position.z / CHUNK_Z);
}

static float oldVerticalAngle = 0.0;
static float oldHorizontalAngle = 0.0;
static vec3_t oldPosition;
static float oldSpeed = 0.0;

void integrate_player(chunk_manager *chunk_m, Player *player, float deltaTime) {
  vec3_t acceleration = vec3(0, -15.0, 0);
  float friction = 0.94;

  // Check if voxel underneath player is solid.
  float feet_y = player->voxel_position.y - 1;
  chunk *currentChunk = find_chunk_by_position(chunk_m, player->chunk_position.x, player->chunk_position.z);

  if (currentChunk != NULL) {

    // Check for solid block underneath.
    // (exception for when the player is jumping)
    if (feet_y < CHUNK_Y && player->state != PLAYER_JUMPING) {
      int block_index = index_from_block(
        (player->voxel_position.x + 0.5),
        feet_y,
        (player->voxel_position.z + 0.5)
      );

      if (currentChunk->blocks[block_index] != 0 &&
          player->state != PLAYER_GROUNDED &&
          player->velocity.y < 0) {
        player->state = PLAYER_GROUNDED;
        player->position.y = round(player->position.y) - 0.001;
      } else if (player->state == PLAYER_GROUNDED) {
        player->state = PLAYER_AIRBORNE;
      }

    } else if (player->state != PLAYER_AIRBORNE) {
      player->state = PLAYER_AIRBORNE;
    }
  }

  if (player->state == PLAYER_GROUNDED) {
    player->velocity = vec3(player->velocity.x * friction, 0, player->velocity.z * friction);
  } else {
    player->state = PLAYER_AIRBORNE;
    player->velocity = vec3(player->velocity.x * friction, player->velocity.y, player->velocity.z * friction);
  }

  player->position = v3_add(player->position,
    v3_muls(
      player->velocity,
      deltaTime
    )
  );

  if (player->state != PLAYER_GROUNDED) {
    player->velocity = v3_add(player->velocity, v3_muls(acceleration, deltaTime));
  }
}

void recalculate_player(Player *player) {
  // Only recalculate if values have changed.
  if (oldVerticalAngle != player->verticalAngle ||
      oldHorizontalAngle != player->horizontalAngle ||
      oldPosition.x != player->position.x ||
      oldPosition.y != player->position.y ||
      oldPosition.z != player->position.z ||
      oldSpeed != player->speed) {

    // Recalculate camera matrix.
    recalculate_camera(
      &(player->cam),
      player->verticalAngle,
      player->horizontalAngle,
      player->position,
      player->speed
    );

    // Recalculate chunk position.
    recalculate_chunk_position(player);
  }
}

void move_player(Player *player, GLenum key, float deltaTime) {
  vec3_t direction = vec3(
    sin(player->horizontalAngle),
    0,
    cos(player->horizontalAngle)
  );

  vec3_t right = vec3(
    sin(player->horizontalAngle - 3.14f/2.0f),
    0,
    cos(player->horizontalAngle - 3.14f/2.0f)
  );

  vec3_t scaled_direction = v3_muls(direction, player->speed * deltaTime);
  vec3_t scaled_right = v3_muls(right, player->speed * deltaTime);

  switch (key) {
    case GLFW_KEY_A:
      player->velocity = v3_sub(player->velocity, scaled_right);
      break;
    case GLFW_KEY_D:
      player->velocity = v3_add(player->velocity, scaled_right);
      break;
    case GLFW_KEY_W:
      player->velocity = v3_add(player->velocity, scaled_direction);
      break;
    case GLFW_KEY_S:
      player->velocity = v3_sub(player->velocity, scaled_direction);
      break;
    case GLFW_KEY_SPACE:
      if (player->state == PLAYER_GROUNDED) {
        player->state = PLAYER_JUMPING;
        player->velocity = vec3(player->velocity.x, player->jumpHeight, player->velocity.z);
      }
      break;
  }

  if (player->velocity.x > player->speed) {
    player->velocity.x = player->speed;
  }

  if (player->velocity.z > player->speed) {
    player->velocity.z = player->speed;
  }
}
