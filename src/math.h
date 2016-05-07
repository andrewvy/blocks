#pragma once

#include <math.h>
#include <GLFW/glfw3.h>

typedef struct mat4_t {
	GLfloat m[16];
} mat4_t;

/*
0 1 2 3
4 5 6 7
8 9 A B
C D E F
*/

typedef union vec4_t {
	 GLfloat m[4];
	 struct {
	 	GLfloat x, y, z, w;
	 };
} vec4_t;

static const vec4_t X_AXIS = {{1, 0, 0, 0}};
static const vec4_t Y_AXIS = {{0, 1, 0, 0}};
static const vec4_t Z_AXIS = {{0, 0, 1, 0}};
static const vec4_t INV_X_AXIS = {{-1, 0, 0, 0}};
static const vec4_t INV_Y_AXIS = {{0, -1, 0, 0}};
static const vec4_t INV_Z_AXIS = {{0, 0, -1, 0}};

static const mat4_t IDENTITY_MATRIX = {{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
}};

mat4_t multiplymat4(const mat4_t* m1, const mat4_t* m2);
vec4_t mulmatvec4(const mat4_t* m, const vec4_t* v);
void normalizevec4(vec4_t* v);
GLfloat dotvec4(vec4_t v1, vec4_t v2);
vec4_t crossvec4(vec4_t v1, vec4_t v2);
void rotateX(const mat4_t* m, GLfloat angle);
void rotateY(const mat4_t* m, GLfloat angle);
void rotateZ(const mat4_t* m, GLfloat angle);
void scale(const mat4_t* m, GLfloat x, GLfloat y, GLfloat z);
void translate(const mat4_t* m, GLfloat x, GLfloat y, GLfloat z);

mat4_t perspective(GLfloat fovy, GLfloat aspect_ratio, GLfloat near_plane, GLfloat far_plane);
mat4_t orthogonal(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top);

mat4_t lookAt(vec4_t pos, vec4_t dir);
