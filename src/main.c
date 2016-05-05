#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include <SDL2/SDL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "stb/stb_easy_font.h"

#define TICKS_PER_SECOND 60
#define TICKRATE 60

const char *GAME_NAME = "blocks";
const char *GAME_VERSION = "0.0.1-dev";

int SCREEN_WIDTH = 640 * 2;
int SCREEN_HEIGHT = 480 * 2;

static bool quit = false;
static bool initialized = false;
static SDL_Window *window;
static SDL_GLContext *context;

float carried_dt = 0;
float global_timer;

float render_time;
static uint64_t start_time, end_time;

void error(char *s) {
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", s, NULL);
  exit(0);
}

static float text_color[3] = { 1.0f, 1.0f, 1.0f };
static float pos_x = 10;
static float pos_y = 10;

float camang[3], camloc[3] = { 60, 22, 77 };
float player_zoom = 1.0;
float rotate_view = 0.0;

float mouse_x;
float mouse_y;

static void print_string(float x, float y, char *text, float r, float g, float b) {
  static char buffer[99999];
  int num_quads;
  num_quads = stb_easy_font_print(x, y, text, NULL, buffer, sizeof(buffer));

  glColor3f(r, g, b);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT, 16, buffer);
  glDrawArrays(GL_QUADS, 0, num_quads * 4);
  glDisableClientState(GL_VERTEX_ARRAY);
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

void set_text_color(int r, int g, int b) {
  text_color[0] = (1.0f/255) * r;
  text_color[1] = (1.0f/255) * g;
  text_color[2] = (1.0f/255) * b;
}

void draw_stats() {
  static uint64_t last_frame_time;
  uint64_t cur_time = SDL_GetPerformanceCounter();
  float frame_time = (cur_time - last_frame_time) / (float) SDL_GetPerformanceFrequency();
  last_frame_time = cur_time;

  stb_easy_font_spacing(-0.75);
  pos_y = 10;

  set_text_color(0, 84, 111);
  print("%s (%s)", GAME_NAME, GAME_VERSION);

  set_text_color(0, 0, 0);
  print("Frame time %3.2fms - CPU frame render time %3.2fms", frame_time*1000, render_time*1000);
  print("Mouse X: %3.2fpx - Mouse Y: %3.2fpx", mouse_x, mouse_y);
}

void draw_main() {
  glEnable(GL_CULL_FACE);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);

#ifdef REVERSE_DEPTH
  glDepthFunc(GL_GREATER);
  glClearDepth(0);
#else
  glDepthFunc(GL_LESS);
  glClearDepth(1);
#endif
  glDepthMask(GL_TRUE);
  glDisable(GL_SCISSOR_TEST);
  glClearColor(0.6f, 0.7f, 0.9f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor3f(1, 1, 1);
  glFrontFace(GL_CW);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

#ifdef REVERSE_DEPTH
  // stbgl_Perspective(player_zoom, 90, 70, 3000, 1.0/16);
#else
  // stbgl_Perspective(player_zoom, 90, 70, 1.0/16, 3000);
#endif

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // stbgl_initCamera_zup_facing_y();

  glRotatef(-camang[0], 1, 0, 0);
  glRotatef(-camang[2], 0, 0, 1);
  glTranslatef(-camloc[0], -camloc[1], -camloc[2]);

  start_time = SDL_GetPerformanceCounter();
  end_time = SDL_GetPerformanceCounter();

  render_time = (end_time - start_time) / (float) SDL_GetPerformanceFrequency();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glDisable(GL_CULL_FACE);

  draw_stats();
}

void process_tick(float dt) {
}

void process_sdl_mouse(SDL_Event *e) {
   mouse_x = (float) e->motion.xrel;
   mouse_y = (float) e->motion.yrel;
}

void process_event(SDL_Event *e) {
  switch (e->type) {
    case SDL_MOUSEMOTION:
      process_sdl_mouse(e);
      break;
    case SDL_QUIT:
      quit = 1;
      break;
    case SDL_KEYDOWN:
      quit = 1;
      break;
  }
}

void draw() {
  draw_main();
  SDL_GL_SwapWindow(window);
}

static float getTimestep(float minimum_time) {
  float elapsedTime;
  double thisTime;
  static double lastTime = -1;

  if (lastTime == -1)
    lastTime = SDL_GetTicks() / 1000.0 - minimum_time;

  for (;;) {
    thisTime = SDL_GetTicks() / 1000.0;
    elapsedTime = (float) (thisTime - lastTime);

    if (elapsedTime >= minimum_time) {
      lastTime = thisTime;
      return elapsedTime;
    }

    SDL_Delay(1);
  }
}

int loopmode(float dt, int real) {
  if (!initialized) return 0;
  if (!real) return 0;

  // Cap at max 6 frames update
  if (dt > 0.075) dt = 0.075;

  global_timer += dt;
  carried_dt += dt;

  while (carried_dt > 1.0/TICKRATE) {
    carried_dt -= 1.0/TICKRATE;
  }

  process_tick(dt);
  draw();

  return 0;
}

int SDL_main(int argc, char **argv) {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE  , 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE , 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

  window = SDL_CreateWindow(
    "blocks",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
  );

  if (!window) error("Couldn't create window");

  context = SDL_GL_CreateContext(window);
  if (!context) error("Couldn't create context");

  SDL_GL_MakeCurrent(window, context);
  SDL_SetRelativeMouseMode(SDL_TRUE);
  SDL_GL_SetSwapInterval(1);

  initialized = true;

  while (!quit) {
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
      process_event(&e);
    }

    loopmode(getTimestep(0.0166f/8), 1);
  }

  return 0;
}

int main(int argc, char *argv[]) {
  SDL_main(argc, argv);
}
