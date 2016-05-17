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

GLuint make_buffer(GLenum target, GLsizei size, void *data) {
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(target, buffer);
  glBufferData(target, size, data, GL_DYNAMIC_DRAW);

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
