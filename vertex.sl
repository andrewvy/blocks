#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 vColor;

uniform float timer;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 fColor;

void main() {
  gl_Position = Projection * View * Model * position;
  fColor = vec3(vColor.x * sin(timer * 5), vColor.y * sin(timer * 5), vColor.z * sin(timer * 5));
}
