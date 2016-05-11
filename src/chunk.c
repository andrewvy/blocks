#include "third-party/math_3d.h"
#include "chunk.h"
#include "gl.h"

void create_chunk_manager(chunk_manager *manager, render_obj *obj[], int object_count) {
  manager->object_types = malloc(sizeof(void *) * object_count);
  manager->object_count = object_count;

  for (int i = 0; i < object_count; i++) {
    manager->object_types[i] = *obj[object_count];
  }
}

block *create_block(render_obj *obj, vec3_t position) {
  block *new_block = malloc(sizeof(block));
  new_block->position = position;
  new_block->object = obj;

  return new_block;
}

void delete_block(block *render_block) {
  free(render_block);
}

chunk *create_chunk(block *block_array[], int block_count, vec3_t position) {
  chunk *new_chunk = malloc(sizeof(chunk));
  new_chunk->position = position;
  new_chunk->block_count = block_count;

  for (int i = 0; i < block_count; i++) {
    new_chunk->blocks[i] = *block_array[i];
  }

  return new_chunk;
}

void render_chunk(GLuint program, chunk *render_chunk) {
  for (int i = 0; i < render_chunk->block_count; i++) {
    block *render_block = &render_chunk->blocks[i];

    mat4_t transform = m4_transpose(m4_translation(vec3((float) (render_block->position.x), 0.0f, (float) (render_block->position.z))));
    render_block->object->transform = transform;
    draw_render_obj(program, render_block->object);
  }
}

void debug_render_chunk(GLuint program, chunk *render_chunk) {
  for (int i = 0; i < render_chunk->block_count; i++) {
    block *render_block = &render_chunk->blocks[i];

    mat4_t transform = m4_transpose(m4_translation(vec3((float) (render_block->position.x), 0.0f, (float) (render_block->position.z))));
    render_block->object->transform = transform;
    debug_draw_render_obj(program, render_block->object);
  }
}
