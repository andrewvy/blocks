#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 vertexUV;

uniform float timer;
uniform mat4 Model;
uniform mat4 ProjectionView;

out vec4 outPosition;
out vec3 outNormal;
out vec2 UV;

void main() {
  outPosition = position;
  outNormal = normal;
  UV = vertexUV;

  gl_Position = ProjectionView * Model * position;
}
