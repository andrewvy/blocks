#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "modern.h"

char *load_file(const char *path) {
  FILE *file = fopen(path, "rb");
  fseek(file, 0, SEEK_END);
  int length = ftell(file);
  rewind(file);
  char *data = calloc(length + 1, sizeof(char));
  fread(data, 1, length, file);
  fclose(file);

  return data;
}

GLuint make_buffer(GLenum target, GLsizei size, const void *data) {
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(target, buffer);
  glBufferData(target, size, data, GL_STATIC_DRAW);
  glBindBuffer(target, 0);

  return buffer;
}

GLuint make_shader(GLenum type, const char *source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

  if (status == GL_FALSE) {
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    GLchar *info = calloc(length, sizeof(GLchar));
    glGetShaderInfoLog(shader, length, NULL, info);
    printf("glCompileShader failed:\n%s\n", info);
    free(info);
  }

  return shader;
}

GLuint load_shader(GLenum type, const char *path) {
  char *data = load_file(path);
  GLuint result = make_shader(type, data);
  free(data);

  return result;
}

GLuint make_program(GLuint shader1, GLuint shader2) {
  GLuint program = glCreateProgram();
  glAttachShader(program, shader1);
  glAttachShader(program, shader2);
  glLinkProgram(program);
  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);

  if (status == GL_FALSE) {
    GLint length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    GLchar *info = calloc(length, sizeof(GLchar));
    glGetProgramInfoLog(program, length, NULL, info);
    fprintf(stderr, "glLinkProgram failed: %s\n", info);
    free(info);
  }

  glDeleteShader(shader1);
  glDeleteShader(shader2);

  return program;
}

GLuint load_program(GLenum type1, GLenum type2, const char *path1, const char *path2) {
  GLuint shader1 = load_shader(type1, path1);
  GLuint shader2 = load_shader(type2, path2);
  GLuint program = make_program(shader1, shader2);

  return program;
}

void identity_matrix(float *matrix) {
  matrix[0] = 1;
  matrix[1] = 0;
  matrix[2] = 0;
  matrix[3] = 0;
  matrix[4] = 0;
  matrix[5] = 1;
  matrix[6] = 0;
  matrix[7] = 0;
  matrix[8] = 0;
  matrix[9] = 0;
  matrix[10] = 1;
  matrix[11] = 0;
  matrix[12] = 0;
  matrix[13] = 0;
  matrix[14] = 0;
  matrix[15] = 1;
}

void frustum_matrix(float *matrix, float left, float right, float bottom, float top, float znear, float zfar) {
  float temp = 2.0 * znear;
  float temp2 = right - left;
  float temp3 = top - bottom;
  float temp4 = zfar - znear;

  matrix[0] = temp / temp2;
  matrix[1] = 0.0;
  matrix[2] = 0.0;
  matrix[3] = 0.0;
  matrix[4] = 0.0;
  matrix[5] = temp / temp3;
  matrix[6] = 0.0;
  matrix[7] = 0.0;
  matrix[8] = (right + left) / temp2;
  matrix[9] = (top + bottom) / temp3;
  matrix[10] = (-zfar - znear) / temp4;
  matrix[11] = -1.0;
  matrix[12] = 0.0;
  matrix[13] = 0.0;
  matrix[14] = (-temp * zfar) / temp4;
  matrix[15] = 0.0;
}

void perspective_matrix(float *matrix, float fov, float aspect, float znear, float zfar) {
  float ymax = znear * tanf(fov * PI / 360.0);
  float xmax = ymax * aspect;
  frustum_matrix(matrix, -xmax, xmax, -ymax, ymax, znear, zfar);
}

void make_cube(float *output, float x, float y, float z, float n) {
  *(output++) = x - n;
  *(output++) = y - n;
  *(output++) = z - n;
  *(output++) = x - n;
  *(output++) = y - n;
  *(output++) = z + n;
  *(output++) = x - n;
  *(output++) = y + n;
  *(output++) = z - n;
  *(output++) = x - n;
  *(output++) = y + n;
  *(output++) = z + n;
  *(output++) = x + n;
  *(output++) = y - n;
  *(output++) = z - n;
  *(output++) = x + n;
  *(output++) = y - n;
  *(output++) = z + n;
  *(output++) = x + n;
  *(output++) = y + n;
  *(output++) = z - n;
  *(output++) = x + n;
  *(output++) = y + n;
  *(output++) = z + n;
}
