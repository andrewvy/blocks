#version 330 core

layout(location = 0) in vec4 position;

uniform float timer;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec4 outPosition;

void main() {
  outPosition = position;
  gl_Position = Projection * View * Model * position;
}
