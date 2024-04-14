#version 430 core

const vec2 vertices[6] = vec2[] (
    vec2(-1, -1),
    vec2(-1,  1),
    vec2( 1,  1),
    vec2( 1,  1),
    vec2( 1, -1),
    vec2(-1, -1)
);

void main() {
  gl_Position = vec4(vertices[gl_VertexID], 1.0, 1.0);
}
