#pragma once

typedef struct {
  GLuint vao_id;

  mat4_t transform;
  GLenum mode; // GL_TRIANGLES
  GLuint indices_count; // Number of indices to draw
  GLuint vbo;

  size_t size;
} render_obj;

typedef struct {
  mat4_t matrix;
  vec3_t center;
  vec3_t up;
} camera;

typedef enum {
  PLAYER_GROUNDED,
  PLAYER_AIRBORNE
} PlayerState;

typedef struct {
  PlayerState state;
  vec3_t position;
  float speed; // Fixed movement speed
  vec3_t velocity;
  float jumpHeight; // Fixed jump height

  camera cam;
  float horizontalAngle;
  float verticalAngle;

  vec3_t voxel_position;
  vec3_t chunk_position;
} Player;

typedef struct {
  GLfloat x;
  GLfloat z;
  uint8_t *blocks;
  render_obj *render_object;
} chunk;

typedef struct {
  chunk **loaded_chunks;
  int chunk_boundary;
  int number_of_loaded_chunks;
} chunk_manager;

