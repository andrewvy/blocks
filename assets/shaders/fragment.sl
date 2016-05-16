#version 330 core

uniform float timer;
uniform sampler2D fragTexture;

in vec4 outPosition;
in vec2 UV;

out vec4 color;

void main() {
  color = texture(fragTexture, UV);
}
