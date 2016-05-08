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

render_obj *render_objects[50];

static mat4_t Model;
static mat4_t View;
static mat4_t Projection;

int render_init() {
  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // Dark blue background
  glClearColor(0.45f, 0.45f, 0.8f, 1.0f);

  // Create and compile our GLSL program from the shaders
  GLuint vertex_shader = load_shader(GL_VERTEX_SHADER, "vertex.sl");
  GLuint fragment_shader = load_shader(GL_FRAGMENT_SHADER, "fragment.sl");
  program = make_program(vertex_shader, fragment_shader);

  // Use our shaders
  glUseProgram(program);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  GLfloat verts[] = {
    //front
    -0.5f, -0.5f,  0.5f, 0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,

    //bottom
    -0.5f, -0.5f, -0.5f, 0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,

    //left
    -0.5f, -0.5f, -0.5f, 0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f, 0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,

    //right
     0.5f,  0.5f, -0.5f, 0.5f,  0.5f, 0.5f,  0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f, 0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,

    //top
    -0.5f,  0.5f,  0.5f, 0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,

    //back
     0.5f, -0.5f, -0.5f, 0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f
  };

  render_objects[0] = create_render_obj(GL_TRIANGLES, verts, 6 * 6 * 3);

  // Reset matrices to identity
  vec4_t camera_pos = {{0}};
  vec4_t camera_foc = {{0, 0, -1, 0}};
  vec4_t camera_up = {{0, 0, 1, 0}};
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
  rotateY(&Model, 0.02);
  rotateZ(&Model, 0.01);

  // Load matrices
  GLint model = glGetUniformLocation(program, "Model");
  glUniformMatrix4fv(model, 1, GL_TRUE, &(Model.m[0]));

  GLint view = glGetUniformLocation(program, "View");
  glUniformMatrix4fv(view, 1, GL_FALSE, &(View.m[0]));

  GLint projection = glGetUniformLocation(program, "Projection");
  glUniformMatrix4fv(projection, 1, GL_FALSE, &(Projection.m[0]));

  // Add timer
  glUniform1f(glGetUniformLocation(program, "timer"), glfwGetTime());

  // Render a render_obj
  draw_render_obj(render_objects[0]);

  // Swap buffers
  glfwSwapBuffers(window);
}

void render_quit() {
  // Check for OpenGL Errors on exit
  // and print to screen.
  check_opengl_error();

  glDeleteProgram(program);
  destroy_render_obj(render_objects[0]);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(void) {
  // Initialise GLFW

  if (video_init() == -1) return -1;
  if (render_init() == -1) return -1;

  glfwSetKeyCallback(window, key_callback);

  while (!glfwWindowShouldClose(window)) {
    render();
    glfwPollEvents();
  }

  return 0;
}
