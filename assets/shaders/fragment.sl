#version 330 core

uniform float timer;
uniform vec3 CamPosition;
uniform sampler2D fragTexture;

in vec3 outNormal;
in vec4 outPosition;
in vec2 UV;

out vec4 color;

void main() {
  vec4 fColor = texture(fragTexture, UV);
  vec4 tintColor = vec4(1.0, 1.0, 1.0, 1.0);

  color = mix(
    fColor,
    tintColor,
    0.5
  );
}
