#include <SDL2/SDL.h>
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "utils.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main() {
	// Inicjalizacja biblioteki SDL2
	SDL_Init(SDL_INIT_EVERYTHING);

	// Utworzenie okna i kontekstu OpenGL
	SDL_Window* window = SDL_CreateWindow("Raymarcher", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	// Załadowanie funkcji OpenGL za pomocą biblioteki glad
	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
	SDL_GL_MakeCurrent(window, context);

	// Debug message callback będzie wypisywał na konsolę błędy OpenGL
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(gl_message_callback, NULL);

	// Kompilowanie shaderów
	GLuint vertex_shader = shader_from_file("vertex_shader.glsl", GL_VERTEX_SHADER);
	GLuint fragment_shader = shader_from_file("fragment_shader.glsl", GL_FRAGMENT_SHADER);
	GLuint shader_program = create_shader_program(vertex_shader, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	if (!shader_program)
		goto exit;

	glUseProgram(shader_program);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// Pusty vertex array object. Istnieje tylko po to, żeby OpenGL nie płakał
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Główna pętla programu
	SDL_Event event;
	while (1) {
		// Obsługa wydarzeń okna
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				goto exit;
			}
		}

		// Tutaj będzie aktualizacja stanu programu
		// ----------------------------------------

		// Renderowanie kolejnej klatki
		glDrawArrays(GL_TRIANGLES, 0, 6);
		SDL_GL_SwapWindow(window);
	}

exit: // Zwalnianie zasobów
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(shader_program);
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
