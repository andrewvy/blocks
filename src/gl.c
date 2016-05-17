#include <stdlib.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gl.h"
#include "modern.h"
#include "third-party/math_3d.h"

// #define BLOCKS_DEBUG

int apply_render_obj_attribute(render_obj *obj, GLuint vbo_id, GLenum buffer_type, int attr_index, int elem_count, GLenum type, GLenum normalized, GLsizei stride, GLvoid *offset) {
  glBindVertexArray(obj->vao_id);
  glEnableVertexAttribArray(attr_index);
  glBindBuffer(buffer_type, vbo_id);
  glVertexAttribPointer(
    attr_index,                  // attribute 0.
    elem_count,                  // size
    type,                        // type
    normalized,                  // normalized?
    stride,                      // stride
    offset                       // array buffer offset
  );

  return 0;
}

render_obj *create_render_obj(const GLenum mode, const GLfloat *buffer_data, int buffer_count, int indices_count) {
  render_obj *obj = calloc(1, sizeof(render_obj));
  size_t buffer_size = sizeof(GLfloat) * buffer_count;

  // Create VAO
  glGenVertexArrays(1, &obj->vao_id);
  glBindVertexArray(obj->vao_id);

  // Set metadata + transform matrix
  obj->indices_count = indices_count;
  obj->mode = mode;
  obj->size = buffer_size;

  // Set model transform.
  obj->transform = m4_identity();

  // Create VBO
  obj->vbo = make_buffer(GL_ARRAY_BUFFER, buffer_size, buffer_data);

  return obj;
}

int destroy_render_obj(render_obj *obj) {
  glBindVertexArray(obj->vao_id);
  glDeleteBuffers(1, &(obj->vbo));
  glDeleteVertexArrays(1, &(obj->vao_id));
  glBindVertexArray(0);
  free(obj);

  return 0;
}

void draw_render_obj(GLuint program, render_obj *obj) {
  glBindVertexArray(obj->vao_id); // Bind VAO

  // Load matrices
  GLint model = glGetUniformLocation(program, "Model");
  glUniformMatrix4fv(model, 1, GL_TRUE, (GLfloat *) &obj->transform);

  glDrawArrays(obj->mode, 0, obj->indices_count); // 3 indices starting at 0 -> 1 triangle

  glBindVertexArray(0); // Clear bounded VAO
}

void debug_draw_render_obj(GLuint program, render_obj *obj) {
  glBindVertexArray(obj->vao_id); // Bind VAO

  // Load matrices
  GLint model = glGetUniformLocation(program, "Model");
  glUniformMatrix4fv(model, 1, GL_TRUE, (GLfloat *) &obj->transform);

  glDrawArrays(GL_LINES, 0, obj->indices_count); // 3 indices starting at 0 -> 1 triangle

  glBindVertexArray(0); // Clear bounded VAO
}
