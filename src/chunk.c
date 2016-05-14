#include <GLFW/glfw3.h>

#include "third-party/math_3d.h"
#include "chunk.h"
#include "gl.h"

chunk *create_chunk(GLfloat x, GLfloat z) {
  chunk *new_chunk = malloc(sizeof(chunk));
  new_chunk->blocks = (uint8_t *) calloc(1, sizeof(uint8_t) * CHUNK_SIZE);

  // Sets all blocks to a non-air type. (1)
  for (int i = 0; i < CHUNK_SIZE; i++) {
    new_chunk->blocks[i] = 1;
  }

  new_chunk->x = x;
  new_chunk->z = z;

  return new_chunk;
}

void generate_chunk_mesh(chunk *render_chunk) {
  GLfloat *buffer = malloc(sizeof(GLfloat) * (6 * 6 * 5) * CHUNK_SIZE);
  GLfloat *buffer_pointer = buffer;
  int vertices_count = 0;

  for (int i = 0; i < CHUNK_SIZE; i++) {
    int x = i % CHUNK_X;
    int z = (i / CHUNK_X) % CHUNK_Z;
    int y = i / (CHUNK_X * CHUNK_Z);

    uint8_t block = render_chunk->blocks[i];

    // left(-x), right(+x), top(+y), bottom(-y), front(-z), back(+z)
    uint8_t neighbors[6] = {1, 1, 1, 1, 1, 1};

    // Calculate neighboring voxels and check if they are solid or not.
    // If they are not solid, we should render a face. Otherwise, it is
    // unnecessary to include the face. This resulting mesh will be
    // contain only the exposed faces. Perf++.
    //
    // TODO(vy): Need to check between chunk boundaries for checking
    // face exposure.
    neighbors[0] = (x - 1 < 0) ? 1 : render_chunk->blocks[i - 1] == 0;
    neighbors[1] = (x + 1 >= CHUNK_X) ? 1 : render_chunk->blocks[i + 1] == 0;
    neighbors[2] = (y + 1 >= CHUNK_Y) ? 1 : render_chunk->blocks[i + (CHUNK_X * CHUNK_Z)] == 0;
    neighbors[3] = (y - 1 < 0) ? 1 : render_chunk->blocks[i - (CHUNK_X * CHUNK_Z)] == 0;
    neighbors[4] = (z - 1 < 0) ? 1 : render_chunk->blocks[i - CHUNK_X] == 0;
    neighbors[5] = (z + 1 >= CHUNK_Z) ? 1 : render_chunk->blocks[i + CHUNK_X] == 0;

    vertices_count += create_cube_mesh(&buffer_pointer,
      neighbors[0], neighbors[1], neighbors[2], neighbors[3], neighbors[4], neighbors[5],
      1, 1, 1, 1, 1, 1,
      x, y, z, 0.5
    );
  }


  // Resize the mesh buffer to the actual size, since we allocated
  // the largest size, but during mesh creation, we could have
  // omitted faces, causing the buffer to be smaller than the
  // maximum size.
  buffer = realloc(buffer, (sizeof(GLfloat) * vertices_count * 5));

  // TODO(vy): Figure out how to calculate vertices of exposed faces:
  // 6 vertices per face * NUMBER OF FACES
  render_chunk->render_object = create_render_obj(
    GL_TRIANGLES,
    buffer,
    (6 * 6 * 5) * CHUNK_SIZE,
    vertices_count
  );

  // Bind Vertices
  apply_render_obj_attribute(
    render_chunk->render_object,                // Render Object
    render_chunk->render_object->buffers[0],    // ID of the VBO
    GL_ARRAY_BUFFER,                  // Type of Buffer (GL_ARRAY_BUFFER)
    0,                                // Attribute Index of the VAO
    3,                                // Number of Elements
    GL_FLOAT,                         // Type of Element (GL_FLOAT)
    GL_FALSE,                         // Whether this is normalized?
    (sizeof(GLfloat) * 5),
    0                                 // Pointer to the offset (void pointer)
  );

  apply_render_obj_attribute(
    render_chunk->render_object,                // Render Object
    render_chunk->render_object->buffers[0],    // ID of the VBO
    GL_ARRAY_BUFFER,                  // Type of Buffer (GL_ARRAY_BUFFER)
    1,                                // Attribute Index of the VAO
    2,                                // Number of Elements
    GL_FLOAT,                         // Type of Element (GL_FLOAT)
    GL_FALSE,                         // Whether this is normalized?
    (sizeof(GLfloat) * 5),
    (GLvoid *) (sizeof(GLfloat) * 3)  // Pointer to the offset (void pointer)
  );

  render_chunk->render_object->transform = m4_transpose(
    m4_translation(
      vec3(
        render_chunk->x * CHUNK_X,
        0,
        render_chunk->z * CHUNK_Z
      )
    )
  );

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

int create_cube_mesh(
    GLfloat **data_pointer,
    int left, int right, int top, int bottom, int front, int back,
    int wleft, int wright, int wtop, int wbottom, int wfront, int wback,
    GLfloat x, GLfloat y, GLfloat z, GLfloat cube_size
  ) {

  int vertices_count = 0;

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

  int faces[6] = {left, right, top, bottom, front, back};

  // UV calculation based on texture size!
  // TODO(vy): CHANGE THESE
  GLfloat s = 0.3025;
  GLfloat a = 0 + 1 / 2048.0;
  GLfloat b = s - 1 / 2048.0;

  // For each face, construct the mesh.
  for (int i = 0; i < 6; i++) {
    // Skip if face is missing.
    if (faces[i] == 0) continue;

    for (int v = 0; v < 6; v++) {
      int j = indices[i][v];
      *((*data_pointer)++) = x + cube_size * vertices[i][j][0];
      *((*data_pointer)++) = y + cube_size * vertices[i][j][1];
      *((*data_pointer)++) = z + cube_size * vertices[i][j][2];
      *((*data_pointer)++) = (uvs[i][j][0] ? b : a);
      *((*data_pointer)++) = (uvs[i][j][1] ? b : a);
      vertices_count++;
    }
  }

  return vertices_count;
}

uint8_t block_from_index(chunk *render_chunk, uint32_t index) {
  return 0;
}

uint32_t index_from_block(uint16_t x, uint16_t y, uint16_t z) {
  // uint modx = x % SIZEX;
  // uint mody = y % SIZEY;
  // uint modz = z % SIZEZ;

  // return (modz * SIZEXY) + (modx * SIZEY) + mody;

  uint32_t x_length = CHUNK_X;
  uint32_t z_length = CHUNK_Z;
  uint32_t y_length = CHUNK_Y;
  if (x > x_length) x = x_length;
  if (y > y_length) y = y_length;
  if (z > z_length) z = z_length;

  uint32_t index = (y * x_length * z_length) + (z * x_length) + x;
  return index;
}

void render_chunk(GLuint program, chunk *render_chunk) {
  draw_render_obj(program, render_chunk->render_object);
}

void debug_render_chunk(GLuint program, chunk *render_chunk) {
  debug_draw_render_obj(program, render_chunk->render_object);
}

int destroy_chunk(chunk *render_chunk) {
  destroy_render_obj(render_chunk->render_object);
  free(render_chunk->blocks);
  free(render_chunk);

  return 0;
}
