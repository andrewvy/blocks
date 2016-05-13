#include <GLFW/glfw3.h>

#include "third-party/math_3d.h"
#include "chunk.h"
#include "gl.h"

chunk *create_chunk(int32_t x, int32_t z) {
  chunk *new_chunk = malloc(sizeof(chunk));
  new_chunk->blocks = (uint8_t *) calloc(1, sizeof(CHUNK_SIZE));

  return new_chunk;
}

// Graciously inspired via https://github.com/fogleman/Craft/blob/master/src/cube.c#L7
// This implementation is 99% identical to the link above.
//
// However, I will try to make this:
// - able to handle different sizes of textures without hardcoding resolution
// - have better documentation around the variables
// ----
//
// This method creates the mesh of the cube, but only with certain
// faces exposed. The resulting mesh (output into &data) is the
// vertices, normals, uvs of ONLY the exposed faces. This impacts
// mem space on the overall mesh significantly.

void create_cube_mesh(
    GLfloat *data,
    int left, int right, int top, int bottom, int front, int back,
    int wleft, int wright, int wtop, int wbottom, int wfront, int wback
  ) {

  static const GLfloat vertices[6][4][3] = {
    {{-1, -1, -1}, {-1, -1, +1}, {-1, +1, -1}, {-1, +1, +1}},
    {{+1, -1, -1}, {+1, -1, +1}, {+1, +1, -1}, {+1, +1, +1}},
    {{-1, +1, -1}, {-1, +1, +1}, {+1, +1, -1}, {+1, +1, +1}},
    {{-1, -1, -1}, {-1, -1, +1}, {+1, -1, -1}, {+1, -1, +1}},
    {{-1, -1, -1}, {-1, +1, -1}, {+1, -1, -1}, {+1, +1, -1}},
    {{-1, -1, +1}, {-1, +1, +1}, {+1, -1, +1}, {+1, +1, +1}}
  };

  static const GLfloat normals[6][3] = {
    {-1, 0, 0},
    {+1, 0, 0},
    {0, +1, 0},
    {0, -1, 0},
    {0, 0, -1},
    {0, 0, +1}
  };

  static const GLfloat uvs[6][4][2] = {
    {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
    {{1, 0}, {0, 0}, {1, 1}, {0, 1}},
    {{0, 1}, {0, 0}, {1, 1}, {1, 0}},
    {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
    {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
    {{1, 0}, {1, 1}, {0, 0}, {0, 1}}
  };

  static const GLfloat indices[6][6] = {
    {0, 3, 2, 0, 1, 3},
    {0, 3, 1, 0, 2, 3},
    {0, 3, 2, 0, 1, 3},
    {0, 3, 1, 0, 2, 3},
    {0, 3, 2, 0, 1, 3},
    {0, 3, 1, 0, 2, 3}
  };

  static const GLfloat flipped[6][6] = {
    {0, 1, 2, 1, 3, 2},
    {0, 2, 1, 2, 3, 1},
    {0, 1, 2, 1, 3, 2},
    {0, 2, 1, 2, 3, 1},
    {0, 1, 2, 1, 3, 2},
    {0, 2, 1, 2, 3, 1}
  };

  GLfloat *data_pointer = data;
  int faces[6] = {left, right, top, bottom, front, back};

  // UV calculation based on texture size!
  // TODO(vy): CHANGE THESE
  GLfloat s = 0.625;
  GLfloat a = 0 + 1 / 2048.0;
  GLfloat b = s - 1 / 2048.0;

  // For each face, construct the mesh.
  for (int i = 0; i < 6; i++) {
    // Skip if face is missing.
    if (faces[i] == 0) continue;

    for (int v = 0; v < 6; v++) {
      int j = indices[i][v];
      *(data_pointer++) = vertices[i][j][0];
      *(data_pointer++) = vertices[i][j][1];
      *(data_pointer++) = vertices[i][j][2];
      *(data_pointer++) = (uvs[i][j][0] ? b : a);
      *(data_pointer++) = (uvs[i][j][1] ? b : a);
    }
  }
}

uint8_t block_from_index(chunk *render_chunk, uint32_t index) {
  return 0;
}

uint32_t index_from_block(uint16_t x, uint16_t y, uint16_t z) {
  uint32_t x_length = CHUNK_WIDTH;
  uint32_t z_length = CHUNK_HEIGHT;
  uint32_t y_length = CHUNK_DEPTH;
  if (x > x_length) x = x_length;
  if (y > y_length) y = y_length;
  if (z > z_length) z = z_length;

  uint32_t index = (y * x_length * z_length) + (z * x_length) + x;
  return index;
}

void render_chunk(GLuint program, chunk *render_chunk) {
}

int destroy_chunk(chunk *render_chunk) {
  free(render_chunk->blocks);
  free(render_chunk);

  return 0;
}
