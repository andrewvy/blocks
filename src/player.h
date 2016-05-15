#pragma once

#include "camera.h"

typedef enum {
  PLAYER_GROUNDED,
  PLAYER_AIRBORNE
} PlayerState;

typedef struct {
  PlayerState state;
  vec3_t position;
  float speed; // Fixed movement speed
  vec3_t velocity;
  float jumpHeight; // Fixed jump height

  camera cam;
  float horizontalAngle;
  float verticalAngle;

  vec3_t chunk_position;
} Player;

Player *create_player();
void destroy_player(Player *player);
void integrate_player(Player *player, float deltaTime);
void recalculate_player(Player *player);
void move_player(Player *player, GLenum key, float deltaTime);
