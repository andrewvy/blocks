#include <stdlib.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gl.h"
#include "modern.h"
#include "math.h"

render_obj *create_render_obj(const GLenum mode, const GLfloat *mesh_data, int mesh_count) {
  render_obj *obj = malloc(sizeof(render_obj));
  size_t mesh_size = sizeof(GLfloat) * mesh_count;

  // Create VAO
  glGenVertexArrays(1, &obj->vao_id);
  glBindVertexArray(obj->vao_id);

  // Create VBO
  obj->vbo_id = make_buffer(GL_ARRAY_BUFFER, mesh_size, mesh_data);
  glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_id);

  // Set metadata + transform matrix
  obj->transform = IDENTITY_MATRIX;
  obj->indices_count = mesh_count;
  obj->mode = mode;
  obj->size = sizeof(GLfloat) * mesh_count;

  // 1st attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    0,                  // attribute 0.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    6 * sizeof(GLfloat), // stride
    (GLvoid *) 0  // array buffer offset
  );

  // 2nd attribute buffer : color info
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
    1,                  // attribute 1.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    6 * sizeof(GLfloat), // stride
    (GLvoid *) (3 * sizeof(GLfloat))  // array buffer offset
  );

  return obj;
}

int destroy_render_obj(render_obj *obj) {
  glBindVertexArray(obj->vao_id);
  glDeleteBuffers(1, &(obj->vbo_id));
  glDeleteVertexArrays(1, &(obj->vao_id));
  free(obj);

  return 0;
}

void draw_render_obj(render_obj *obj) {
  glBindVertexArray(obj->vao_id); // Bind VAO
  glDrawArrays(obj->mode, 0, obj->indices_count); // 3 indices starting at 0 -> 1 triangle
  glBindVertexArray(0); // Clear bounded VAO
}
