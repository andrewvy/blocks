#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "third-party/linmath.h"
#include "modern.h"
#include "gl.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

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
  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Modern OpenGL", NULL, NULL);

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

static mat4x4 Model;
static mat4x4 View;
static mat4x4 Projection;

static vec3 cam_eye = { 3, 3, 3 };
static vec3 cam_center = { 0, 0, 0 };
static vec3 cam_up = { 0, 1, 0 };

int render_init() {
  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,

    1.0f, 1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,

    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,

    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,

    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,

    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,

    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,

    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,

    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,

    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,

    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,

    1.0f,  1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f
  };

  render_objects[0] = create_render_obj(GL_TRIANGLES, verts, 3 * 3 * 12);

  // Reset matrices to identity
  mat4x4_identity(Model);
  mat4x4_look_at(View, cam_eye, cam_center, cam_up);

  mat4x4_perspective(Projection, 45.0, 4.0 / 3.0, 0.1f, 20.0f);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);

  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  check_opengl_error();

  return 0;
}

void render() {
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Load matrices
  GLint model = glGetUniformLocation(program, "Model");
  glUniformMatrix4fv(model, 1, GL_TRUE, (GLfloat *) Model);

  GLint view = glGetUniformLocation(program, "View");
  glUniformMatrix4fv(view, 1, GL_FALSE, (GLfloat *) View);

  GLint projection = glGetUniformLocation(program, "Projection");
  glUniformMatrix4fv(projection, 1, GL_FALSE, (GLfloat *) Projection);

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

static float horizontalAngle = 3.95f;
static float verticalAngle = -0.6f;
static float speed = 3.0f;
static float mouseSpeed = 0.02f;

void integrate(double time, float deltaTime) {
  double xpos, ypos;

  glfwGetCursorPos(window, &xpos, &ypos);
  glfwSetCursorPos(window, WINDOW_WIDTH/2, WINDOW_HEIGHT/2);

  horizontalAngle += mouseSpeed * deltaTime * ((float) (WINDOW_WIDTH/2 - floor(xpos)));
  verticalAngle += mouseSpeed * deltaTime * ((float) (WINDOW_HEIGHT/2 - floor(ypos)));

  vec3 direction = {
    cos(verticalAngle) * sin(horizontalAngle),
    sin(verticalAngle),
    cos(verticalAngle) * cos(horizontalAngle)
  };

  vec3 right = {
    sin(horizontalAngle - 3.14f/2.0f),
    0,
    cos(horizontalAngle - 3.14f/2.0f)
  };

  vec3 up;
  vec3_mul_cross(up, right, direction);

  vec3 position_direction;
  vec3_add(position_direction, cam_eye, direction);

  mat4x4_look_at(View, cam_eye, position_direction, up);
}

int main(void) {
  // Initialise GLFW

  if (video_init() == -1) return -1;
  if (render_init() == -1) return -1;

  glfwSetKeyCallback(window, key_callback);

  double t = 0.0;
  double dt = 1 / 60.0;
  double currentTime = glfwGetTime();

  glfwSwapInterval(1);

  while (!glfwWindowShouldClose(window)) {
    double newTime = glfwGetTime();
    double frameTime = newTime - currentTime;
    currentTime = newTime;

    while (frameTime > 0.0) {
      float deltaTime;

      if (frameTime < dt) {
        deltaTime = frameTime;
      } else {
        deltaTime = dt;
      }

      integrate(t, deltaTime);
      frameTime -= deltaTime;
      t += deltaTime;
    }

    render();
    glfwPollEvents();
  }

  return 0;
}
