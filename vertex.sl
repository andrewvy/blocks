#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 vertexUV;

uniform float timer;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec4 outPosition;
out vec2 UV;

void main() {
  outPosition = position;
  UV = vertexUV;

  gl_Position = Projection * View * Model * position;
}
