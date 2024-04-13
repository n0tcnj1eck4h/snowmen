#version 430 core

const vec2 vertices[6] = vec2[] (
    vec2(-1, -1),
    vec2(-1,  1),
    vec2( 1,  1),
    vec2( 1,  1),
    vec2( 1, -1),
    vec2(-1, -1)
);

out vec2 position;

void main() {
  position = vertices[gl_VertexID];
  gl_Position = vec4(position, 1.0, 1.0);
}
