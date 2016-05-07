#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "modern.h"
#include "math.h"
#include "gl.h"

GLFWwindow *window;

GLuint VertexArrayID;
GLuint vertexbuffer;
GLuint program;

// Checks for OpenGL errors
void check_opengl_error() {
  GLenum err;
  char* err_s;

  while ((err  = glGetError()) != GL_NO_ERROR) {
    switch(err) {
      case GL_INVALID_OPERATION:
        err_s = "INVALID_OPERATION";
        break;
      case GL_INVALID_ENUM:
        err_s = "INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        err_s = "INVALID_VALUE";
        break;
      case GL_OUT_OF_MEMORY:
        err_s = "OUT_OF_MEMORY";
        break;
    }

    printf("OpenGL error: %s\n", err_s);
  }
}

int video_init() {
  if(!glfwInit()) {
    printf("Failed to initialize GLFW\n");
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(1024, 768, "Modern OpenGL", NULL, NULL);

  if (window == NULL) {
    printf("Failed to open GLFW window.\n" );
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

  // Initialize GLEW (Needed for core profile)
  glewExperimental = GL_TRUE;

  if (glewInit() != GLEW_OK) {
    printf("Failed to initialize GLEW.\n");
    return -1;
  }

  return 0;
}

vbo_mesh *triangle;

static mat4_t Model;
static mat4_t View;
static mat4_t Projection;

int render_init() {
  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // Dark blue background
  glClearColor(0.45f, 0.45f, 0.8f, 1.0f);

  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  GLfloat triangle_data[] = {
    -0.5f, -0.5f, 0.0f, 0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f,
    0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 0.5f
  };

  triangle = create_vbo_mesh(triangle_data, 6 * 3);

  // Create and compile our GLSL program from the shaders
  GLuint vertex_shader = load_shader(GL_VERTEX_SHADER, "vertex.sl");
  GLuint fragment_shader = load_shader(GL_FRAGMENT_SHADER, "fragment.sl");
  program = make_program(vertex_shader, fragment_shader);

  // Use our shaders
  glUseProgram(program);

  vertexbuffer = make_buffer(GL_ARRAY_BUFFER, sizeof(GLfloat) * triangle->vertex_count, triangle->buffer);

  // Reset matrices to identity
  Model = IDENTITY_MATRIX;
  View = IDENTITY_MATRIX;
  Projection = IDENTITY_MATRIX;

  check_opengl_error();

  return 0;
}

void render() {
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT);

  // Apply rotation on the model
  rotateX(&Model, 0.02);

  // Load matrices
  GLint model = glGetUniformLocation(program, "Model");
  glUniformMatrix4fv(model, 1, GL_TRUE, &(Model.m[0]));

  GLint view = glGetUniformLocation(program, "View");
  glUniformMatrix4fv(view, 1, GL_FALSE, &(View.m[0]));

  GLint projection = glGetUniformLocation(program, "Projection");
  glUniformMatrix4fv(projection, 1, GL_FALSE, &(Projection.m[0]));

  // Add timer
  glUniform1f(glGetUniformLocation(program, "timer"), glfwGetTime());

  // 1st attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

  glVertexAttribPointer(
    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    6 * sizeof(GLfloat),                  // stride
    (GLvoid *) 0  // array buffer offset
  );

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
    1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    6 * sizeof(GLfloat),                  // stride
    (GLvoid *) (3 * sizeof(GLfloat))  // array buffer offset
  );

  // Draw the triangle !
  glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);

  // Swap buffers
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void render_quit() {
  // Check for OpenGL Errors on exit
  // and print to screen.
  check_opengl_error();

  glDeleteBuffers(1, &vertexbuffer);
  glDeleteVertexArrays(1, &VertexArrayID);
  glDeleteProgram(program);
  destroy_vbo_mesh(triangle);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();
}

int main(void) {
  // Initialise GLFW

  if (video_init() == -1) return -1;
  if (render_init() == -1) return -1;

  while (!glfwWindowShouldClose(window)) {
    render();
  }

  return 0;
}
