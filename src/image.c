#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image.h"

// Quick and dirty way to load BMP textures.
GLuint loadBMPImage(const char *image) {
  unsigned char header[54];
  unsigned int dataPos;
  unsigned int imageSize;
  unsigned int width;
  unsigned int height;
  unsigned char *data;

  FILE *file = fopen(image, "rb");

  if (!file) {
    printf("%s could not be opened.\n", image);
    return 0;
  }

  if (fread(header, 1, 54, file) != 54) {
    printf("%s is not a correct BMP file\n", image);
    return 0;
  }

  if (*(int *) &(header[0x1E]) != 0) {
    printf("%s is not a correct BMP file\n", image);
    return 0;
  }

  if (*(int *) &(header[0x1C]) != 24) {
    printf("%s is not a correct BMP file\n", image);
    return 0;
  }

  dataPos   = *(int *) &(header[0x0A]);
  imageSize = *(int *) &(header[0x22]);
  width     = *(int *) &(header[0x12]);
  height    = *(int *) &(header[0x16]);

  if (imageSize == 0) imageSize = width * height * 3;
  if (dataPos == 0) dataPos = 54;

  data = (unsigned char *) malloc(imageSize);

  fread(data, 1, imageSize, file);
  fclose(file);

  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);

  free(data);

  return textureID;
}
