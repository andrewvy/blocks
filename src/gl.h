#pragma once

#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct {
  GLfloat *buffer;
  uint32_t vertex_count;
} vbo_mesh;

typedef struct {
  uint32_t mesh_count;
  vbo_mesh* meshes;
} vbo_meshes;

vbo_mesh *create_vbo_mesh(const GLfloat *mesh_data, int mesh_count);
int destroy_vbo_mesh(vbo_mesh *mesh);
