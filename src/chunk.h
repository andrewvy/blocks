#pragma once

#include <stdint.h>

#include "third-party/math_3d.h"
#include "gl.h"
#include "player.h"

#define CHUNK_X 16
#define CHUNK_Z 16
#define CHUNK_Y 256
#define CHUNK_SIZE CHUNK_X * CHUNK_Z * CHUNK_Y
#define LOADED_CHUNK_BOUNDARY 6

typedef struct {
  GLfloat x;
  GLfloat z;
  uint8_t *blocks;
  render_obj *render_object;
} chunk;

typedef struct {
  chunk **loaded_chunks;
  int chunk_boundary;
  int number_of_loaded_chunks;
} chunk_manager;

chunk *create_chunk(GLfloat x, GLfloat z);
void generate_chunk_mesh(chunk *render_chunk);

int create_cube_mesh(
    GLfloat **data_pointer,
    int left, int right, int top, int bottom, int front, int back,
    int wleft, int wright, int wtop, int wbottom, int wfront, int wback,
    GLfloat x, GLfloat y, GLfloat z, GLfloat cube_size);

void render_chunk(GLuint program, chunk *render_chunk);
void debug_render_chunk(GLuint program, chunk *render_chunk);
int destroy_chunk(chunk *render_chunk);

chunk_manager *create_chunk_manager();
void chunk_manager_process(chunk_manager *cm, Player *player);
void destroy_chunk_manager(chunk_manager *cm);
