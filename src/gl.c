#include <stdlib.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gl.h"
#include "modern.h"
#include "third-party/math_3d.h"

// #define BLOCKS_DEBUG

int apply_render_obj_attribute(render_obj *obj, GLuint vbo_id, GLenum buffer_type, int attr_index, int elem_count, GLenum type, GLenum normalized, GLvoid *offset) {
  glBindVertexArray(obj->vao_id);
  glEnableVertexAttribArray(attr_index);
  glBindBuffer(buffer_type, vbo_id);
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

  // Set model transform.
  obj->transform = m4_identity();

  // Create VBO
  obj->vbo_id = make_buffer(GL_ARRAY_BUFFER, mesh_size, mesh_data);
  apply_render_obj_attribute(
    obj,             // Render Object
    obj->vbo_id,     // ID of the VBO
    GL_ARRAY_BUFFER, // Type of Buffer (GL_ARRAY_BUFFER)
    0,               // Attribute Index of the VAO
    3,               // Number of Elements
    GL_FLOAT,        // Type of Element (GL_FLOAT)
    GL_FALSE,        // Whether this is normalized?
    (void *) 0       // Pointer to the offset (void pointer)
  );

  // Create UV VBO
  obj->uv_vbo_id = make_buffer(GL_ARRAY_BUFFER, uv_size, uv_data);
  apply_render_obj_attribute(
    obj,             // Render Object
    obj->uv_vbo_id,  // ID of the VBO
    GL_ARRAY_BUFFER, // Type of Buffer (GL_ARRAY_BUFFER)
    1,               // Attribute Index of the VAO
    2,               // Number of Elements
    GL_FLOAT,        // Type of Element (GL_FLOAT)
    GL_FALSE,        // Whether this is normalized?
    (void *) 0       // Pointer to the offset (void pointer)
  );

  return obj;
}

int destroy_render_obj(render_obj *obj) {
  glBindVertexArray(obj->vao_id);
  glDeleteBuffers(1, &(obj->vbo_id));
  glDeleteBuffers(1, &(obj->uv_vbo_id));
  glDeleteVertexArrays(1, &(obj->vao_id));
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
