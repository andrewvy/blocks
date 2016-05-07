#include <stdlib.h>

#include "gl.h"

vbo_mesh create_vbo_mesh(const char *mesh_data, int mesh_count) {
  vbo_mesh mesh;

  mesh.vertex_build_buffer = *mesh_data;
  mesh.vertex_count = mesh_count;

  return mesh;
}

int destroy_vbo_mesh(vbo_mesh mesh) {
  return 0;
}
