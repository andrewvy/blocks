#version 330 core

uniform float timer;

in vec4 outPosition;
out vec3 color;

void main() {
  color = vec3(
    sin(timer * 1) / abs(outPosition.z),
    sin(timer * 0.66) / abs(outPosition.z),
    sin(timer * 0.33) / abs(outPosition.z)
  );
}
