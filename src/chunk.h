#pragma once

#include <stdint.h>
#include "third-party/math_3d.h"
#include "gl.h"

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 16
#define CHUNK_DEPTH 256
#define CHUNK_SIZE CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH
#define LOADED_CHUNK_BOUNDARY 3

typedef struct {
  int32_t x;
  int32_t z;
  uint8_t *blocks;
} chunk;

typedef struct {
  chunk *loaded_chunks[LOADED_CHUNK_BOUNDARY];
} chunk_manager;

chunk *create_chunk(int32_t x, int32_t z);
void create_cube_mesh(
    GLfloat *data,
    int left, int right, int top, int bottom, int front, int back,
    int wleft, int wright, int wtop, int wbottom, int wfront, int wback);
int destroy_chunk(chunk *render_chunk);
