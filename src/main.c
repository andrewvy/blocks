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
#include "player.h"

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

static chunk_manager *ChunkManager;
static mat4_t Projection;
static Player *player;
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
  player = create_player();

  // Load the texture using any two methods
  Texture = loadPNGImage("assets/textures/blocks.png");

  // Create Chunk Manager
  ChunkManager = create_chunk_manager();

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

  fonsSetSize(fontcontext, 16.0f); // 16 point font
  fonsSetAlign(fontcontext, FONS_ALIGN_LEFT | FONS_ALIGN_TOP); // left/top aligned
  fonsVertMetrics(fontcontext, NULL, NULL, &lineHeight);

  check_opengl_error();

  return 0;
}

static unsigned char BLOCKS_DEBUG = 0;

void render_gui() {
  int vertex_count = 0;

  for (int i = 0; i < ChunkManager->number_of_loaded_chunks; i++) {
    vertex_count += ChunkManager->loaded_chunks[0]->render_object->indices_count;
  }

  gl3fonsProjection(fontcontext, mat);

  char gui_title[] = "Blocks 0.0.1-dev";
  char gui_vertex_count[256];
  char gui_player_position[256];
  char gui_player_look[256];
  char gui_player_velocity[256];
  char gui_chunk_position[256];

  sprintf(gui_vertex_count, "Vertex Count: %2d", vertex_count);
  gui_vertex_count[255] = '\0';

  sprintf(gui_player_position, "Player Position <x: %2.2f, y: %2.2f, z: %2.2f>", player->position.x, player->position.y, player->position.z);
  gui_player_position[255] = '\0';

  sprintf(gui_player_look, "Player Look <Vertical: %2.2f, Horizontal: %2.2f>", player->verticalAngle, player->horizontalAngle);
  gui_player_look[255] = '\0';

  sprintf(gui_player_velocity, "Player Velocity <x: %2.2f, y: %2.2f, z: %2.2f>", player->velocity.x, player->velocity.y, player->velocity.z);
  gui_player_velocity[255] = '\0';

  sprintf(gui_chunk_position, "Chunk Position <x: %2.2f, z: %2.2f>", player->chunk_position.x, player->chunk_position.z);
  gui_player_position[255] = '\0';

  fonsSetColor(fontcontext, gl3fonsRGBA(255, 255, 255, 255)); // white
  fonsDrawText(fontcontext, 10.0f, 10.0f, gui_title, NULL);
  fonsDrawText(fontcontext, 10.0f, 40.0f, gui_vertex_count, NULL);
  fonsDrawText(fontcontext, 10.0f, 70.0f, gui_player_position, NULL);
  fonsDrawText(fontcontext, 10.0f, 100.0f, gui_player_look, NULL);
  fonsDrawText(fontcontext, 10.0f, 130.0f, gui_player_velocity, NULL);
  fonsDrawText(fontcontext, 10.0f, 160.0f, gui_chunk_position, NULL);

  if (BLOCKS_DEBUG) {
    char gui_debug_mode[] = "DEBUG MODE ON";
    fonsSetColor(fontcontext, gl3fonsRGBA(250, 85, 85, 255));
    fonsDrawText(fontcontext, 10.0f, 190.0f, gui_debug_mode, NULL);
  }
}

void render() {
  // Use our shaders
  glUseProgram(program);

  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Calculate camera and set View uniform.
  recalculate_player(player);
  mat4_t ProjectionView = m4_mul(Projection, player->cam.matrix);
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

  for (int i = 0; i < ChunkManager->number_of_loaded_chunks; i++) {
    if (BLOCKS_DEBUG) {
      debug_render_chunk(program, ChunkManager->loaded_chunks[i]);
    } else {
      render_chunk(program, ChunkManager->loaded_chunks[i]);
    }
  }

  // Enable face culling
  glDisable(GL_CULL_FACE);

  // Enable depth test
  glDisable(GL_DEPTH_TEST);

  // Render GUI
  render_gui();

  // Swap buffers
  glfwSwapBuffers(window);
}

void render_quit() {
  // Check for OpenGL Errors on exit
  // and print to screen.
  check_opengl_error();

  glDeleteProgram(program);
  glDeleteTextures(1, &TextureID);

  destroy_chunk_manager(ChunkManager);
  destroy_player(player);

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

  player->horizontalAngle += mouseSpeed * deltaTime * ((float) (WINDOW_WIDTH/2 - floor(xpos)));
  player->verticalAngle += mouseSpeed * deltaTime * ((float) (WINDOW_HEIGHT/2 - floor(ypos)));
}

void keyInput(double time, float deltaTime) {
  if (glfwGetKey(window, GLFW_KEY_A)) move_player(player, GLFW_KEY_A, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D)) move_player(player, GLFW_KEY_D, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_W)) move_player(player, GLFW_KEY_W, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S)) move_player(player, GLFW_KEY_S, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_SPACE)) move_player(player, GLFW_KEY_SPACE, deltaTime);
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
      integrate_player(player, deltaTime);

      frameTime -= deltaTime;
      ongoingTime += deltaTime;
    }

    chunk_manager_process(ChunkManager, player);
    render();
    glfwPollEvents();
  }

  return 0;
}
