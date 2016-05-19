#pragma once

#include "common.h"

Player *create_player();
void destroy_player(Player *player);
void integrate_player(chunk_manager *chunk_m, Player *player, float deltaTime);
void recalculate_player(Player *player);
void move_player(Player *player, GLenum key, float deltaTime);
