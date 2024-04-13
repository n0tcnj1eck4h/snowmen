#version 430 core

layout(location = 0) out vec4 color;
uniform vec2 screen_size;
uniform vec3 dir; // = normalize(vec3(-2.0, -2.0, -2.0));

in vec2 position;

const float EPSILON = 0.0001;
const uint MAX_ITERATIONS = 1024;
const float FOCAL_LENGTH = 0.1;
const vec3 UP = vec3(0,1,0);

vec3 cam = vec3(2.0, 2.0, 2.0);

float sphereSDF(vec3 cam, vec3 pos, float r) {
  return distance(pos, cam) - r;
}

float sceneSDF(vec3 cam) {
  return sphereSDF(cam, vec3(0,0,0), 1);
}

vec3 estimateNormal(vec3 cam) {
  return normalize(vec3(
    sceneSDF(vec3(cam.x + EPSILON, cam.y, cam.z)) - sceneSDF(vec3(cam.x - EPSILON, cam.y, cam.z)),
    sceneSDF(vec3(cam.x, cam.y + EPSILON, cam.z)) - sceneSDF(vec3(cam.x, cam.y - EPSILON, cam.z)),
    sceneSDF(vec3(cam.x, cam.y, cam.z  + EPSILON)) - sceneSDF(vec3(cam.x, cam.y, cam.z - EPSILON))
  ));             
}

void main() {
  vec3 right = normalize(cross(dir, UP));
  vec3 up = normalize(cross(right, dir));
  vec3 ray_dir = dir + position.x * right + position.y * up * screen_size.y / screen_size.x;
  for(uint i = 0; i < MAX_ITERATIONS; i++) {
    float dist = sceneSDF(cam);
    if(dist < EPSILON) {
      color = vec4(estimateNormal(cam), 1.0) * 1.0;
      return;
    }
    else {
      cam = cam + ray_dir * dist;
    }
  }
  
      color = vec4(0.0, 1.0, 1.0, 1.0);
}
