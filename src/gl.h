#pragma once

#include <stdlib.h>

typedef struct {
  char *vertex_build_buffer;
  uint32_t vertex_count;
} vbo_mesh;

typedef struct {
  uint32_t mesh_count;
  vbo_mesh* meshes;
} vbo_meshes;

vbo_mesh create_vbo_mesh(const char *mesh_data, int mesh_count);
int destroy_vbo_mesh(vbo_mesh mesh);
