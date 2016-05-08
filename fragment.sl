#version 330 core

uniform float timer;

out vec3 color;

void main() {
	color = vec3(sin(timer * 1), sin(timer * 0.66), sin(timer * 0.33));
}
