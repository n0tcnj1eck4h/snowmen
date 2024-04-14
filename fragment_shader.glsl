#version 430 core

layout(location = 0) out vec4 color;
const vec2 SCREEN_SIZE = vec2(800, 600);

void main() {
  color = vec4(gl_FragCoord.xy / SCREEN_SIZE, 0.0, 1.0);
}
