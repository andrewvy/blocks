#pragma once

#include <stdlib.h>
#include <GLFW/glfw3.h>

#include "third-party/linmath.h"

typedef struct {
  GLuint vbo_id;
  GLuint uv_vbo_id;
  GLuint vao_id;
  mat4x4 *transform;
  GLenum mode; // GL_TRIANGLES
  GLuint indices_count; // Number of indices to draw
  size_t size;
} render_obj;

render_obj *create_render_obj(const GLenum mode, const GLfloat *mesh_data, int mesh_count, const GLfloat *uv_data, int uv_count);
int destroy_render_obj(render_obj *obj);
void draw_render_obj(render_obj *obj);
