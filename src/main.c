#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;
SDL_Surface *gHelloWorld = NULL;

bool init() {
  bool success = true;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize!");
    success = false;
  } else {
    gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (gWindow == NULL) {
      printf("Window could not be created!");
      success = false;
    } else {
      gScreenSurface = SDL_GetWindowSurface(gWindow);
    }
  }

  return success;
}

bool loadMedia() {
  bool success = true;
  gHelloWorld = SDL_LoadBMP("test.bmp");

  if (gHelloWorld == NULL) {
    printf("Unable to load image!");
    success = false;
  }

  return success;
}

void close() {
  SDL_FreeSurface(gHelloWorld);
  gHelloWorld = NULL;

  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  SDL_Quit();
}

int main (int argc, char *argv[]) {
  bool isRunning = true;
  SDL_Event e;

  if (!init()) {
    printf("Failed to initialize!\n");
  } else {
    if (!loadMedia()) {
      printf("Failed to load media!");
    } else {
      SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
      SDL_UpdateWindowSurface(gWindow);

      while (isRunning) {
        while (SDL_PollEvent(&e) != 0) {
          if (e.type == SDL_QUIT) {
            isRunning = false;
          }
        }
      }
    }
  }

  close();

  return 0;
}
