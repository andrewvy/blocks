#pragma once
#include "third-party/math_3d.h"

#include <stdlib.h>
#include <stdint.h>
#include <GLFW/glfw3.h>

typedef struct {
  GLuint vao_id;

  mat4_t transform;
  GLenum mode; // GL_TRIANGLES
  GLuint indices_count; // Number of indices to draw
  GLuint vbo;

  size_t size;
} render_obj;

render_obj *create_render_obj(const GLenum mode, const GLfloat *buffer_data, int buffer_count, int indices_count);
int apply_render_obj_attribute(render_obj *obj, GLuint vbo_id, GLenum buffer_type, int attr_index, int elem_count, GLenum type, GLenum normalized, GLsizei stride, GLvoid *offset);
int destroy_render_obj(render_obj *obj);
void draw_render_obj(GLuint program, render_obj *obj);
void debug_draw_render_obj(GLuint program, render_obj *obj);
