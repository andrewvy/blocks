#pragma once

#include <stdlib.h>
#include <GLFW/glfw3.h>

#include "math.h"

typedef struct {
  GLuint vbo_id;
  GLuint vao_id;
  mat4_t transform;
  GLenum mode; // GL_TRIANGLES
  GLuint indices_count; // Number of indices to draw
  size_t size;
} render_obj;

render_obj *create_render_obj(const GLenum mode, const GLfloat *mesh_data, int mesh_count);
int destroy_render_obj(render_obj *obj);
void draw_render_obj(render_obj *obj);
