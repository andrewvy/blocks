#version 330 core

uniform float timer;
uniform sampler2D fragTexture;

in vec3 outNormal;
in vec4 outPosition;
in vec2 UV;

out vec4 color;

void main() {
  float cosTheta = clamp(dot(outNormal, vec3(0, 1, 0)), 0, 1);
  vec4 LightColor = vec4(1, 1, 1, 1);

  color = texture(fragTexture, UV) * LightColor * cosTheta;
}
