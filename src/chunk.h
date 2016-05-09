#pragma once

#include <stdint.h>

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 16
#define CHUNK_DEPTH 256
#define CHUNK_SIZE CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH

typedef struct {
  uint8_t id;
  uint8_t x;
  uint8_t y;
  uint8_t z;
  uint8_t w;
} block;

typedef struct {
  block blocks[CHUNK_SIZE];
  float x;
  float y;
  float z;
} chunk;
