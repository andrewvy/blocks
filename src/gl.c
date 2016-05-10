#include <stdlib.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gl.h"
#include "modern.h"
#include "third-party/linmath.h"

// #define BLOCKS_DEBUG

render_obj *create_render_obj(const GLenum mode, const GLfloat *mesh_data, int mesh_count, const GLfloat *uv_data, int uv_count) {
  render_obj *obj = malloc(sizeof(render_obj));

  size_t mesh_size = sizeof(GLfloat) * mesh_count;
  size_t uv_size = sizeof(GLfloat) * uv_count;

  // Create VAO
  glGenVertexArrays(1, &obj->vao_id);
  glBindVertexArray(obj->vao_id);

  // Set metadata + transform matrix
  obj->indices_count = mesh_count;
  obj->mode = mode;
  obj->size = sizeof(GLfloat) * mesh_count;

  // Create VBO
  obj->vbo_id = make_buffer(GL_ARRAY_BUFFER, mesh_size, mesh_data);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_id);
  glVertexAttribPointer(
    0,           // attribute 0.
    3,           // size
    GL_FLOAT,    // type
    GL_FALSE,    // normalized?
    0,           // stride
    (void *) 0 // array buffer offset
  );

  // Create UV VBO
  obj->uv_vbo_id = make_buffer(GL_ARRAY_BUFFER, uv_size, uv_data);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, obj->uv_vbo_id);
  glVertexAttribPointer(
    1,           // attribute 1.
    2,           // size
    GL_FLOAT,    // type
    GL_FALSE,    // normalized?
    0,           // stride
    (void *) 0 // array buffer offset
  );

  return obj;
}

int apply_render_obj_attribute(render_obj *obj, int attr_index, GLenum type, GLenum normalized, int elem_count, GLvoid *offset) {
  glBindVertexArray(obj->vao_id);
  glEnableVertexAttribArray(attr_index);
  glVertexAttribPointer(
    attr_index,                  // attribute 0.
    elem_count,                  // size
    type,                        // type
    normalized,                  // normalized?
    elem_count * sizeof(type),   // stride
    offset                       // array buffer offset
  );

  return 0;
}

int destroy_render_obj(render_obj *obj) {
  glBindVertexArray(obj->vao_id);
  glDeleteBuffers(1, &(obj->vbo_id));
  glDeleteBuffers(1, &(obj->uv_vbo_id));
  glDeleteVertexArrays(1, &(obj->vao_id));
  free(obj);

  return 0;
}

void draw_render_obj(render_obj *obj) {
  glBindVertexArray(obj->vao_id); // Bind VAO

#ifdef BLOCKS_DEBUG
  glDrawArrays(GL_LINES, 0, obj->indices_count); // 3 indices starting at 0 -> 1 triangle
#else
  glDrawArrays(obj->mode, 0, obj->indices_count); // 3 indices starting at 0 -> 1 triangle
#endif

  glBindVertexArray(0); // Clear bounded VAO
}
