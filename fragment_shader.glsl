#version 430 core

// Wartość zmiennej color po zakończeniu funkcji main odpowiada 
// ostatecznemu kolorowi piksela.
// Cztery wymiary wektora odpowiadają kolejno barwom rgba
out vec4 color;

// Rozmiar okna w pikselach. Używany do normalizacji pozycji piksela
const vec2 SCREEN_SIZE = vec2(800, 600);
const vec3 UP = vec3(0,1,0);
const float MAX_DISTANCE = 1024.0;
const uint MAX_ITERATIONS = 1024;
const float EPSILON = 0.001;

uniform vec3 camera_position;
uniform vec3 camera_direction;

float sphereSDF(vec3 p, float r) {
  return length(p) - r;
}

float sceneSDF(vec3 p) {
  return sphereSDF(p, 1);
}

vec3 estimateNormal(vec3 ray) {
  return normalize(vec3(
    sceneSDF(vec3(ray.x + EPSILON, ray.y, ray.z)) - sceneSDF(vec3(ray.x - EPSILON, ray.y, ray.z)),
    sceneSDF(vec3(ray.x, ray.y + EPSILON, ray.z)) - sceneSDF(vec3(ray.x, ray.y - EPSILON, ray.z)),
    sceneSDF(vec3(ray.x, ray.y, ray.z  + EPSILON)) - sceneSDF(vec3(ray.x, ray.y, ray.z - EPSILON))
  ));             
}

void main() {
  vec3 right = normalize(cross(camera_direction, UP));
  vec3 up = normalize(cross(right, camera_direction));

  vec2 pixel_position = gl_FragCoord.xy / SCREEN_SIZE * 2 - vec2(1,1);
  float aspect_ratio = SCREEN_SIZE.x / SCREEN_SIZE.y;
  vec3 ray_direction = normalize(camera_direction 
                                 + pixel_position.x * right * aspect_ratio
                                 + pixel_position.y * up);

  float total_distance = 0.0;
  for(uint i = 0; i < MAX_ITERATIONS; i++) {
    vec3 ray_position = camera_position + total_distance * ray_direction;
    float dist = sceneSDF(ray_position);

    if(dist < 0.001) {
      color = vec4(estimateNormal(ray_position),1);
      return;
    }
    total_distance += dist;
    if(total_distance > MAX_DISTANCE) break;
  }

  // Nic nie trafiliśmy, czarny piksel
  color = vec4(0,0,0,1);
}
