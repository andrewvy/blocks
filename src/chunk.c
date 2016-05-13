#include <GLFW/glfw3.h>

#include "third-party/math_3d.h"
#include "chunk.h"
#include "gl.h"

chunk *create_chunk(int32_t x, int32_t z) {
  chunk *new_chunk = malloc(sizeof(chunk));
  new_chunk->blocks = (uint8_t *) calloc(1, sizeof(CHUNK_SIZE));

  return new_chunk;
}

float create_cube_mesh(uint8_t material_id) {
  vec3_t vertices[24] = {
    vec3(-1, -1, -1), vec3(-1, -1, +1), vec3(-1, +1, -1), vec3(-1, +1, +1),
    vec3(+1, -1, -1), vec3(+1, -1, +1), vec3(+1, +1, -1), vec3(+1, +1, +1),
    vec3(-1, +1, -1), vec3(-1, +1, +1), vec3(+1, +1, -1), vec3(+1, +1, +1),
    vec3(-1, -1, -1), vec3(-1, -1, +1), vec3(+1, -1, -1), vec3(+1, -1, +1),
    vec3(-1, -1, -1), vec3(-1, +1, -1), vec3(+1, -1, -1), vec3(+1, +1, -1),
    vec3(-1, -1, +1), vec3(-1, +1, +1), vec3(+1, -1, +1), vec3(+1, +1, +1)
  };

  vec3_t normals[6] = {
    vec3(-1, 0, 0),
    vec3(+1, 0, 0),
    vec3(0, +1, 0),
    vec3(0, -1, 0),
    vec3(0, 0, -1),
    vec3(0, 0, +1)
  };

  float uvs[6][4][2] = {
    {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
    {{1, 0}, {0, 0}, {1, 1}, {0, 1}},
    {{0, 1}, {0, 0}, {1, 1}, {1, 0}},
    {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
    {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
    {{1, 0}, {1, 1}, {0, 0}, {0, 1}}
  };

  return 0.0;
}

uint8_t block_from_index(chunk *render_chunk, uint32_t index) {
  return 0;
}

uint32_t index_from_block(uint16_t x, uint16_t y, uint16_t z) {
  uint32_t x_length = CHUNK_WIDTH;
  uint32_t z_length = CHUNK_HEIGHT;
  uint32_t y_length = CHUNK_DEPTH;
  if (x > x_length) x = x_length;
  if (y > y_length) y = y_length;
  if (z > z_length) z = z_length;

  uint32_t index = (y * x_length * z_length) + (z * x_length) + x;
  return index;
}

void render_chunk(GLuint program, chunk *render_chunk) {
}

int destroy_chunk(chunk *render_chunk) {
  free(render_chunk->blocks);
  free(render_chunk);

  return 0;
}
