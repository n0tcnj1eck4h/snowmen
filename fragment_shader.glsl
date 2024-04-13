#version 430 core

layout(location = 0) out vec4 color;

in vec2 position;

const float EPSILON = 0.0001;
const uint MAX_ITERATIONS = 1024;
const float FOCAL_LENGTH = 0.1;
const vec3 UP = vec3(0,1,0);

vec3 cam = vec3(2.0, 2.0, 2.0);
vec3 dir = normalize(vec3(-2.0, -2.0, -2.0));

// Funkcja zwacająca dystans do kuli o promieniu r na pozycji pos względem kamery o pozycji cam
float sphereSDF(vec3 pos, float r) {
  return distance(pos, cam) - r;
}

void main() {
  vec3 right = normalize(cross(dir, UP));
  vec3 up = normalize(cross(right, dir));
  vec3 ray_dir = dir + position.x * right + position.y * up;
  for(uint i = 0; i < MAX_ITERATIONS; i++) {
    float dist = sphereSDF(vec3(0,0,0), 1);
    if(dist < EPSILON) {
      color = vec4(1.0, 1.0, 1.0, 1.0);
      return;
    }
    else {
      cam = cam + ray_dir * dist;
    }
  }
  
      color = vec4(0.0, 1.0, 1.0, 1.0);
}
