#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <GLFW/glfw3.h>

#include "third-party/math_3d.h"
#include "common.h"

render_obj *create_render_obj(const GLenum mode, const GLfloat *buffer_data, int buffer_count, int indices_count);
int apply_render_obj_attribute(render_obj *obj, GLuint vbo_id, GLenum buffer_type, int attr_index, int elem_count, GLenum type, GLenum normalized, GLsizei stride, GLvoid *offset);
int destroy_render_obj(render_obj *obj);
void draw_render_obj(GLuint program, render_obj *obj);
void debug_draw_render_obj(GLuint program, render_obj *obj);
