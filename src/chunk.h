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
  GLfloat x;
  GLfloat z;
  uint8_t *blocks;
  render_obj *render_object;
} chunk;

typedef struct {
  chunk *loaded_chunks[LOADED_CHUNK_BOUNDARY];
} chunk_manager;

chunk *create_chunk(GLfloat x, GLfloat z);
void generate_chunk_mesh(chunk *render_chunk);
void create_cube_mesh(
    GLfloat **data_pointer,
    int left, int right, int top, int bottom, int front, int back,
    int wleft, int wright, int wtop, int wbottom, int wfront, int wback,
    GLfloat x, GLfloat y, GLfloat z, GLfloat cube_size);
void render_chunk(GLuint program, chunk *render_chunk);
void debug_render_chunk(GLuint program, chunk *render_chunk);
int destroy_chunk(chunk *render_chunk);
