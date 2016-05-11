#pragma once
#include "third-party/math_3d.h"

#include <stdlib.h>
#include <GLFW/glfw3.h>

typedef struct {
  GLuint vbo_id;
  GLuint uv_vbo_id;
  GLuint vao_id;
	mat4_t transform;
  GLenum mode; // GL_TRIANGLES
  GLuint indices_count; // Number of indices to draw
  size_t size;
} render_obj;

render_obj *create_render_obj(const GLenum mode, const GLfloat *mesh_data, int mesh_count, const GLfloat *uv_data, int uv_count);
int destroy_render_obj(render_obj *obj);
void draw_render_obj(GLuint program, render_obj *obj);
void debug_draw_render_obj(GLuint program, render_obj *obj);
