#version 430 core

// Wartość zmiennej color po zakończeniu funkcji main odpowiada 
// ostatecznemu kolorowi piksela.
// Cztery wymiary wektora odpowiadają kolejno barwom rgba
out vec4 color;

// Rozmiar okna w pikselach. Używany do normalizacji pozycji piksela
const vec2 SCREEN_SIZE = vec2(800, 600);

void main() {
  // Zapis jest jednoznaczny z 
  // color = vec4(gl_FragCoord.x / SCREEN_SIZE.x, gl_FragCoord.y / SCREEN_SIZE.y, 0.0, 1.0)
  color = vec4(gl_FragCoord.xy / SCREEN_SIZE, 0.0, 1.0);
}
