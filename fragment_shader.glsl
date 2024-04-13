#version 430 core

layout(location = 0) out vec4 color;

in vec2 position;

void main() {
  color = vec4(position, 0.0, 1.0);
}
