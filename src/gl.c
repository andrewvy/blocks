#include <stdlib.h>
#include <stdio.h>

#include "gl.h"

vbo_mesh *create_vbo_mesh(const GLfloat *mesh_data, int mesh_count) {
  vbo_mesh *mesh = malloc(sizeof(vbo_mesh));
  GLfloat *alloc_mesh_data = malloc(sizeof(GLfloat) * mesh_count);

  for (int i = 0; i < mesh_count; i++) {
    alloc_mesh_data[i] = mesh_data[i];
  }

  mesh->buffer = alloc_mesh_data;
  mesh->vertex_count = mesh_count;

  return mesh;
}

int destroy_vbo_mesh(vbo_mesh *mesh) {
  if (mesh == NULL) return 0;

  free(mesh->buffer);
  free(mesh);

  return 0;
}
