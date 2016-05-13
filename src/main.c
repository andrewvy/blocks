#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gl.h"
#include "modern.h"
#include "camera.h"
#include "chunk.h"
#include "image.h"

#define MATH_3D_IMPLEMENTATION
#include "third-party/math_3d.h"

#define WINDOW_NAME "blocks"
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
  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, NULL, NULL);

  if (window == NULL) {
    printf("Failed to open GLFW window.\n");
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

static camera Camera;

static GLuint Texture;
static GLuint TextureID;

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

  // Set viewport
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  // Initialize camera
  init_camera(&Camera);

  // Load the texture using any two methods
  Texture = loadBMPImage("texture.bmp");

  // Generate cube mesh which has top of the block unrendered.
  GLfloat data[6 * 6 * 5];
  create_cube_mesh(data,
      1, 1, 0, 1, 1, 1,
      1, 1, 1, 1, 1, 1);

  // TODO(vy): Figure out how to calculate vertices of exposed faces:
  // 6 vertices per face * NUMBER OF FACES
  render_objects[0] = create_render_obj(GL_TRIANGLES, data, 6 * 6 * 5, 6 * 5);

  // Bind Vertices
  apply_render_obj_attribute(
    render_objects[0],                // Render Object
    render_objects[0]->buffers[0],    // ID of the VBO
    GL_ARRAY_BUFFER,                  // Type of Buffer (GL_ARRAY_BUFFER)
    0,                                // Attribute Index of the VAO
    3,                                // Number of Elements
    GL_FLOAT,                         // Type of Element (GL_FLOAT)
    GL_FALSE,                         // Whether this is normalized?
    (sizeof(GLfloat) * 5),
    0                                 // Pointer to the offset (void pointer)
  );

  apply_render_obj_attribute(
    render_objects[0],                // Render Object
    render_objects[0]->buffers[0],    // ID of the VBO
    GL_ARRAY_BUFFER,                  // Type of Buffer (GL_ARRAY_BUFFER)
    1,                                // Attribute Index of the VAO
    2,                                // Number of Elements
    GL_FLOAT,                         // Type of Element (GL_FLOAT)
    GL_FALSE,                         // Whether this is normalized?
    (sizeof(GLfloat) * 5),
    (GLvoid *) (sizeof(GLfloat) * 3)  // Pointer to the offset (void pointer)
  );

  // Set Projection matrix to perspective, with 1 rad FoV
  Projection = m4_perspective(80.0f, width / height, 0.1f, 200.0f);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);

  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  check_opengl_error();

  return 0;
}

static unsigned char BLOCKS_DEBUG = 0;

void render() {
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Calculate camera and set View uniform.
  recalculate_camera(&Camera);
  mat4_t ProjectionView = m4_mul(Projection, Camera.matrix);
  GLint projection_view = glGetUniformLocation(program, "ProjectionView");
  glUniformMatrix4fv(projection_view, 1, GL_FALSE, (GLfloat *) &ProjectionView);

  // Add timer
  glUniform1f(glGetUniformLocation(program, "timer"), glfwGetTime());

  // Add texture
  TextureID = glGetUniformLocation(program, "fragTexture");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, Texture);
  glUniform1i(TextureID, 0);

  // Render a render_obj
  if (BLOCKS_DEBUG) {
    debug_draw_render_obj(program, render_objects[0]);
  } else {
    draw_render_obj(program, render_objects[0]);
  }

  // Swap buffers
  glfwSwapBuffers(window);
}

void render_quit() {
  // Check for OpenGL Errors on exit
  // and print to screen.
  check_opengl_error();

  glDeleteProgram(program);
  glDeleteTextures(1, &TextureID);

  destroy_render_obj(render_objects[0]);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
  if (key == GLFW_KEY_1 && action == GLFW_PRESS) BLOCKS_DEBUG = !BLOCKS_DEBUG;
}

static float mouseSpeed = 0.02f;

void mouseInput(double time, float deltaTime) {
  double xpos, ypos;

  glfwGetCursorPos(window, &xpos, &ypos);
  glfwSetCursorPos(window, WINDOW_WIDTH/2, WINDOW_HEIGHT/2);

  Camera.horizontalAngle += mouseSpeed * deltaTime * ((float) (WINDOW_WIDTH/2 - floor(xpos)));
  Camera.verticalAngle += mouseSpeed * deltaTime * ((float) (WINDOW_HEIGHT/2 - floor(ypos)));
}

void keyInput(double time, float deltaTime) {
  if (glfwGetKey(window, GLFW_KEY_A)) move_camera(&Camera, GLFW_KEY_A, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D)) move_camera(&Camera, GLFW_KEY_D, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_W)) move_camera(&Camera, GLFW_KEY_W, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S)) move_camera(&Camera, GLFW_KEY_S, deltaTime);
}

static double ongoingTime = 0.0;
static double tickrate = 1 / 60.0;
static double currentTime = 0.0;

int main(void) {
  // Initialise GLFW
  if (video_init() == -1) return -1;
  if (render_init() == -1) return -1;

  glfwSetKeyCallback(window, key_callback);
  glfwSwapInterval(1);

  currentTime = glfwGetTime();

  while (!glfwWindowShouldClose(window)) {
    double newTime = glfwGetTime();
    double frameTime = newTime - currentTime;
    currentTime = newTime;

    while (frameTime > 0.0) {
      float deltaTime = 0.0;

      if (frameTime < tickrate) {
        deltaTime = frameTime;
      } else {
        deltaTime = tickrate;
      }

      mouseInput(ongoingTime, deltaTime);
      keyInput(ongoingTime, deltaTime);

      frameTime -= deltaTime;
      ongoingTime += deltaTime;
    }

    render();
    glfwPollEvents();
  }

  return 0;
}
