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

#define FONTSTASH_IMPLEMENTATION
#include "third-party/fontstash.h"

#define GLFONTSTASH_IMPLEMENTATION
#include "third-party/gl3fontstash.h"

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

static chunk *Chunks[4];
static mat4_t Model;
static mat4_t View;
static mat4_t Projection;

static camera Camera;

static GLuint Texture;
static GLuint TextureID;

// FONTS
static GLfloat mat[16];
static FONScontext *fontcontext;
static int SourceCodeProRegular = 0;
static float lineHeight = 0.0f;

int render_init() {
  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Dark blue background
  glClearColor(0.45f, 0.45f, 0.8f, 1.0f);

  // Create and compile our GLSL program from the shaders
  GLuint vertex_shader = load_shader(GL_VERTEX_SHADER, "assets/shaders/vertex.sl");
  GLuint fragment_shader = load_shader(GL_FRAGMENT_SHADER, "assets/shaders/fragment.sl");
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
  Texture = loadBMPImage("assets/textures/texture.bmp");

  // Create chunk
  Chunks[0] = create_chunk(0.0, 0.0);
  Chunks[1] = create_chunk(0.0, -1.0);
  Chunks[2] = create_chunk(-1.0, 0.0);
  Chunks[3] = create_chunk(-1.0, -1.0);

  // Generate cube mesh which has top of the block unrendered.
  generate_chunk_mesh(Chunks[0]);
  generate_chunk_mesh(Chunks[1]);
  generate_chunk_mesh(Chunks[2]);
  generate_chunk_mesh(Chunks[3]);

  // Set Projection matrix to perspective, with 1 rad FoV
  Projection = m4_perspective(80.0f, width / height, 0.1f, 200.0f);

  // Set text rendering matrix
  memset(mat, 0, 16 * sizeof(GLfloat));
  mat[0] = 2.0 / width;
  mat[5] = -2.0 / height;
  mat[10] = 2.0;
  mat[12] = -1.0;
  mat[13] = 1.0;
  mat[14] = -1.0;
  mat[15] = 1.0;

  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  // Add fonts
  fontcontext = gl3fonsCreate(1024, 1024, FONS_ZERO_TOPLEFT);
  SourceCodeProRegular = fonsAddFont(fontcontext, "sans", "assets/fonts/SourceCodePro-Regular.ttf");

  fonsSetSize(fontcontext, 36.0f); // 16 point font
  fonsSetAlign(fontcontext, FONS_ALIGN_LEFT | FONS_ALIGN_TOP); // left/top aligned
  fonsVertMetrics(fontcontext, NULL, NULL, &lineHeight);

  check_opengl_error();

  return 0;
}

static unsigned char BLOCKS_DEBUG = 0;

void render() {
  // Use our shaders
  glUseProgram(program);

  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Calculate camera and set View uniform.
  recalculate_camera(&Camera);
  mat4_t ProjectionView = m4_mul(Projection, Camera.matrix);
  GLint projection_view = glGetUniformLocation(program, "ProjectionView");
  glUniformMatrix4fv(projection_view, 1, GL_FALSE, (GLfloat *) &ProjectionView);

  // Enable face culling
  glEnable(GL_CULL_FACE);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);

  // Add timer
  glUniform1f(glGetUniformLocation(program, "timer"), glfwGetTime());

  // Add texture
  TextureID = glGetUniformLocation(program, "fragTexture");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, Texture);
  glUniform1i(TextureID, 0);

  // Render a render_obj
  // TODO(vy): Store number of loaded chunks.
  int CHUNK_COUNT = 4;

  for (int i = 0; i < CHUNK_COUNT; i++) {
    if (BLOCKS_DEBUG) {
      debug_render_chunk(program, Chunks[i]);
    } else {
      render_chunk(program, Chunks[i]);
    }
  }

  // Enable face culling
  glDisable(GL_CULL_FACE);

  // Enable depth test
  glDisable(GL_DEPTH_TEST);

  // Render text
  int vertex_count = 0;
  for (int i = 0; i < CHUNK_COUNT; i++) {
    vertex_count += Chunks[i]->render_object->indices_count;
  }

  gl3fonsProjection(fontcontext, mat);

  char gui_title[] = "Blocks 0.0.1-dev";
  char gui_vertex_count[256];
  char gui_camera_position[256];

  sprintf(gui_vertex_count, "Vertex Count: %2d", vertex_count);
  gui_vertex_count[255] = '\0';

  sprintf(gui_camera_position, "Camera Position <x: %2.2f, y: %2.2f, z: %2.2f>", Camera.position.x, Camera.position.y, Camera.position.z);
  gui_camera_position[255] = '\0';

  fonsSetColor(fontcontext, gl3fonsRGBA(255, 255, 255, 255)); // white
  fonsDrawText(fontcontext, 10.0f, 10.0f, gui_title, NULL);
  fonsDrawText(fontcontext, 10.0f, 40.0f, gui_vertex_count, NULL);
  fonsDrawText(fontcontext, 10.0f, 70.0f, gui_camera_position, NULL);

  if (BLOCKS_DEBUG) {
    char gui_debug_mode[] = "DEBUG MODE ON";
    fonsSetColor(fontcontext, gl3fonsRGBA(250, 85, 85, 255));
    fonsDrawText(fontcontext, 10.0f, 100.0f, gui_debug_mode, NULL);
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

  for (int i = 0; i < 4; i++) {
    destroy_chunk(Chunks[i]);
  }

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
