#pragma once

#include "camera.h"

typedef struct {
  vec3_t position;

  float speed;

  camera cam;
  float horizontalAngle;
  float verticalAngle;

  vec3_t chunk_position;
} Player;

Player *create_player();
void destroy_player(Player *player);
void recalculate_player(Player *player);
void move_player(Player *player, GLenum key, float deltaTime);
