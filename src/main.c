#include "cglm/types.h"
#include "cglm/vec3.h"
#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_keyboard.h>
#include <SDL_scancode.h>
#include <assert.h>
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "utils.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define UP ((vec3){0, 1, 0})

int main() {
	assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Window* window = SDL_CreateWindow("Raymarcher", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	assert(window != NULL);

	SDL_GLContext context = SDL_GL_CreateContext(window);
	assert(context != NULL);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		printf("Failed to initialize Glad\n");
		return -1;
	}

	SDL_GL_MakeCurrent(window, context);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(gl_message_callback, 0);

	GLuint vertex_shader = shader_from_file("vertex_shader.glsl", GL_VERTEX_SHADER);
	GLuint fragment_shader = shader_from_file("fragment_shader.glsl", GL_FRAGMENT_SHADER);
	GLuint shader_program = create_shader_program(vertex_shader, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	glUseProgram(shader_program);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	vec3 camera_direction = {0, 0, 1};
	GLuint camera_direction_uniform = glGetUniformLocation(shader_program, "dir");
	glUniform3fv(camera_direction_uniform, 1, camera_direction);

	vec3 camera_position = {0, 0, 0};
	GLuint camera_position_uniform = glGetUniformLocation(shader_program, "cam_uniform");
	glUniform3fv(camera_position_uniform, 1, camera_position);

	bool capturing_input = false;
	float sensitivity = 0.005;
	float camera_speed = 1.0;
	float last_time = (float)SDL_GetTicks64();

	const uint8_t* keyboard_state = SDL_GetKeyboardState(NULL);

	SDL_Event event;
	while (1) {
		uint64_t time = (float)SDL_GetTicks64();
		float delta = (time - last_time) / 1000.0f;
		last_time = time;

		while (SDL_PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_QUIT:
				goto exit;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_r:
					printf("Recompiling shaders...\n");
					glUseProgram(0);
					glDeleteProgram(shader_program);
					GLuint vertex_shader = shader_from_file("vertex_shader.glsl", GL_VERTEX_SHADER);
					GLuint fragment_shader = shader_from_file("fragment_shader.glsl", GL_FRAGMENT_SHADER);
					shader_program = create_shader_program(vertex_shader, fragment_shader);
					glDeleteShader(vertex_shader);
					glDeleteShader(fragment_shader);
					if (shader_program != -1) {
						printf("Shaders compiled successfully\n");
						glUseProgram(shader_program);
						camera_position_uniform = glGetUniformLocation(shader_program, "cam_uniform");
						camera_direction_uniform = glGetUniformLocation(shader_program, "dir");
						glUniform3fv(camera_position_uniform, 1, camera_position);
						glUniform3fv(camera_direction_uniform, 1, camera_direction);
					}
					break;
				case SDLK_ESCAPE:
					capturing_input = false;
					SDL_SetRelativeMouseMode(SDL_FALSE);
					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				capturing_input = true;
				SDL_SetRelativeMouseMode(SDL_TRUE);
				break;
			case SDL_MOUSEMOTION:
				if (!capturing_input)
					break;
				vec3 right;
				glm_cross(camera_direction, UP, right);
				glm_vec3_normalize(right);
				glm_vec3_rotate(camera_direction, -event.motion.yrel * sensitivity, right);
				glm_vec3_rotate(camera_direction, -event.motion.xrel * sensitivity, UP);
				glUniform3fv(camera_direction_uniform, 1, camera_direction);
				break;
			case SDL_MOUSEWHEEL:
				camera_speed += (float)event.wheel.y / 5.0;
				if (camera_speed < 1.0)
					camera_speed = 1.0;
				if (camera_speed > 10.0)
					camera_speed = 10.0;
				break;
			}
		}

		if (capturing_input) {
			if (keyboard_state[SDL_SCANCODE_W]) {
				vec3 delta_movement;
				glm_vec3_scale(camera_direction, delta * camera_speed, delta_movement);
				glm_vec3_add(camera_position, delta_movement, camera_position);
			}

			if (keyboard_state[SDL_SCANCODE_S]) {
				vec3 delta_movement;
				glm_vec3_scale(camera_direction, -delta * camera_speed, delta_movement);
				glm_vec3_add(camera_position, delta_movement, camera_position);
			}

			if (keyboard_state[SDL_SCANCODE_D]) {
				vec3 right, delta_movement;
				glm_vec3_cross(camera_direction, UP, right);
				glm_normalize(right);
				glm_vec3_scale(right, delta * camera_speed, delta_movement);
				glm_vec3_add(camera_position, delta_movement, camera_position);
			}

			if (keyboard_state[SDL_SCANCODE_A]) {
				vec3 right, delta_movement;
				glm_vec3_cross(camera_direction, UP, right);
				glm_normalize(right);
				glm_vec3_scale(right, -delta * camera_speed, delta_movement);
				glm_vec3_add(camera_position, delta_movement, camera_position);
			}

			if (keyboard_state[SDL_SCANCODE_SPACE]) {
				vec3 delta_movement;
				glm_vec3_scale(UP, delta * camera_speed, delta_movement);
				glm_vec3_add(camera_position, delta_movement, camera_position);
			}

			if (keyboard_state[SDL_SCANCODE_LCTRL]) {
				vec3 delta_movement;
				glm_vec3_scale(UP, -delta * camera_speed, delta_movement);
				glm_vec3_add(camera_position, delta_movement, camera_position);
			}
		}

		glUniform3fv(camera_position_uniform, 1, camera_position);

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		SDL_GL_SwapWindow(window);
	}

exit:
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(shader_program);
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
