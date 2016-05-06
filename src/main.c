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

GLFWwindow *window;

static GLfloat g_vertex_buffer_data[1000] = {
  -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
   1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
   0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

  -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
   0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
   0.0f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f
};

static float text_color[3] = { 1.0f, 1.0f, 1.0f };
static float pos_x = 10;
static float pos_y = 10;

GLuint VertexArrayID;
GLuint vertexbuffer;
GLuint program;

static void print_string(float x, float y, char *text, float r, float g, float b) {
}

static void print(char *text, ...) {
  char buffer[999];
  va_list va;
  va_start(va, text);
  vsprintf(buffer, text, va);
  va_end(va);
  print_string(pos_x, pos_y, buffer, text_color[0], text_color[1], text_color[2]);
  pos_y += 10;
}

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Modern OpenGL", NULL, NULL);

	if (window == NULL) {
		printf("Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Initialize GLEW (Needed for core profile)
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		printf("Failed to initialize GLEW\n");
		return -1;
	}

  return 0;
}

int render_init() {
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.45f, 0.45f, 0.8f, 1.0f);

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
  GLuint vertex_shader = load_shader(GL_VERTEX_SHADER, "vertex.sl");
  GLuint fragment_shader = load_shader(GL_FRAGMENT_SHADER, "fragment.sl");
  program = make_program(vertex_shader, fragment_shader);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

  return 0;
}

void render() {
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDepthFunc(GL_LESS);
  glClearDepth(1);

  // Use our shader
  glUseProgram(program);

  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

  glVertexAttribPointer(
    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
    4,                  // size
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
  glDrawArrays(GL_TRIANGLES, 3, 3); // 3 indices starting at 0 -> 1 triangle
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
