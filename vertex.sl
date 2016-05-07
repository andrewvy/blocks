#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 vColor;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 fColor;

void main() {
  gl_Position = position;
  fColor = vColor;
}
