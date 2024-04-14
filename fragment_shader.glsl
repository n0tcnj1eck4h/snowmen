#version 430 core

layout(location = 0) out vec4 color;
uniform vec3 dir;
uniform vec3 cam_uniform;

const vec2 SCREEN_SIZE = vec2(800, 600);
const float EPSILON = 0.0001;
const uint MAX_ITERATIONS = 1024;
const float MAX_DISTANCE = 2048.0;
const float FOCAL_LENGTH = 1;
const vec3 UP = vec3(0,1,0);
const vec3 LIGHT_DIRECTION = normalize(vec3(-1,-1,-1));

// Funkcje zniekształcające przestrzeń
vec3 mirror(vec3 ray, vec3 n) {
  return ray - 2 * min(dot(ray, n), 0) * n;
}

vec3 repeat(vec3 ray, vec3 d) {
  return mod(ray + d, d*2) - d;
}


vec3 translate(vec3 ray, vec3 delta) {
  return ray - delta;
}

// Operacje Bool'a
float intersection(float a, float b) {
  return max(a, b);
}

float compliment(float a) { 
  return -a;
}

float subtract(float a, float b) {
  return max(a, -b);
}

// Funckje SDF brył
float sphereSDF(vec3 ray, float r) {
  return length(ray) - r;
}

float boxSDF(vec3 ray, vec3 b) {
  vec3 q = abs(ray) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float cylinderSDF(vec3 ray, float r) {
  return length(ray.xz)-r;
}

float planeSDF(vec3 ray) {
  return ray.y;
}

float coneSDF(vec3 p, float angle, float h) {
  vec2 c = vec2(cos(angle), sin(angle));
  float q = length(p.xz);
  return max(dot(c.xy,vec2(q,p.y)),-h-p.y);
}

float sceneSDF(vec3 ray) {
  float ret;

  float floor = planeSDF(ray + vec3(0,0.5,0));

  ray = repeat(ray, vec3(3,2000,1.5));

  float bottom_sphere  = sphereSDF(ray, 1);
  float middle_sphere  = sphereSDF(ray - vec3(0, 1.25, 0), 0.75);
  float top_sphere  = sphereSDF(ray - vec3(0, 2.25, 0), 0.5);
  float hands = boxSDF(ray - vec3(0, 1.50, 0), vec3(2, 0.1, 0.1));

  float hat_top = cylinderSDF(ray, 0.3);
  hat_top = intersection(hat_top, boxSDF(ray - vec3(0, 2.25, 0), vec3(1)));

  float hat_bottom = cylinderSDF(ray, 0.5);
  hat_bottom = intersection(hat_bottom, boxSDF(ray - vec3(0, 2.60, 0), vec3(2, 0.05, 2)));

  float hat = min(hat_top, hat_bottom);

  float cone = coneSDF((ray - vec3(0,2.3,0.9)).xzy, 0.2, 0.5);

  ray = mirror(ray, vec3(1,0,0));
  float eyes = sphereSDF(ray - vec3(0.2, 2.4, 0.4), 0.1);

  ret = min(bottom_sphere, middle_sphere);
  ret = min(ret, floor);
  ret = min(ret, top_sphere);
  ret = min(ret, hands);
  ret = subtract(ret, eyes);
  ret = min(ret, hat);
  ret = min(ret, cone);
  return ret;
}

vec3 estimateNormal(vec3 ray) {
  return normalize(vec3(
    sceneSDF(vec3(ray.x + EPSILON, ray.y, ray.z)) - sceneSDF(vec3(ray.x - EPSILON, ray.y, ray.z)),
    sceneSDF(vec3(ray.x, ray.y + EPSILON, ray.z)) - sceneSDF(vec3(ray.x, ray.y - EPSILON, ray.z)),
    sceneSDF(vec3(ray.x, ray.y, ray.z  + EPSILON)) - sceneSDF(vec3(ray.x, ray.y, ray.z - EPSILON))
  ));             
}

void main() {
  float aspect_ratio = SCREEN_SIZE.x / SCREEN_SIZE.y;
  vec3 right = normalize(cross(dir, UP));
  vec3 up = normalize(cross(right, dir));
  vec3 ray_dir = normalize(dir * FOCAL_LENGTH
                               + (gl_FragCoord.x / SCREEN_SIZE.x * 2 - 1) * right * aspect_ratio
                               + (gl_FragCoord.y / SCREEN_SIZE.y * 2 - 1) * up);
  vec3 ray_pos = cam_uniform;
  float bloom = dot(dir, ray_dir);
  float total_distance = 0.0;
  for(uint i = 0; i < MAX_ITERATIONS; i++) {
    float dist = sceneSDF(ray_pos);

    total_distance += dist;
    if(total_distance > MAX_DISTANCE) break;

    if(dist < 0.0001) {
      vec3 normal = estimateNormal(ray_pos);
      float ambient_occlusion = max(1.0 - float(i) / 128, 0.1);
      float light = max(dot(normal, -LIGHT_DIRECTION), 0.1);
      float fog = 1.0 - total_distance / MAX_DISTANCE;
      color = vec4(vec3(1) * light * ambient_occlusion * fog * bloom, 1.0);
      return;
    }
    else {
      ray_pos = ray_pos + ray_dir * dist;
    }
  }

  color = vec4(vec3(0.0, 1.0, 1.0) * bloom, 1.0);
}
