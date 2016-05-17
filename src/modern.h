#pragma once

#include <GLFW/glfw3.h>

#define PI 3.14159265359
#define DEGREES(radians) ((radians) * 180 / PI)
#define RADIANS(degrees) ((degrees) * PI / 180)

GLuint make_buffer(GLenum target, GLsizei size, void *data);
GLuint make_shader(GLenum type, const char *source);
GLuint load_shader(GLenum type, const char *path);
GLuint make_program(GLuint shader1, GLuint shader2);
GLuint load_program(GLenum type1, GLenum type2, const char *path1, const char *path2);
