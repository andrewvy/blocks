#pragma once

#include <stdint.h>
#include "third-party/math_3d.h"
#include "gl.h"

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 16
#define CHUNK_DEPTH 256
#define CHUNK_SIZE CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH

typedef struct {
  uint8_t id;
  render_obj *object;
	vec3_t position;
} block;

typedef struct {
  block blocks[CHUNK_SIZE];
	vec3_t position;
  int block_count;
} chunk;

typedef struct {
  render_obj *object_types;
  int object_count;
} chunk_manager;

void create_chunk_manager(chunk_manager *manager, render_obj *obj[], int object_count);
block *create_block(render_obj *obj, vec3_t position);
chunk *create_chunk(block *block_array[], int block_count, vec3_t position);
void destroy_block(block *render_block);
void destroy_chunk(chunk *render_chunk);
void render_chunk(GLuint program, chunk *render_chunk);
void debug_render_chunk(GLuint program, chunk *render_chunk);
