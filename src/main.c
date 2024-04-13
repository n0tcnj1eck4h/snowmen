#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_video.h>
#include <assert.h>
#include <glad/glad.h>
#include <stdio.h>

static char* read_entire_file(const char* filename) {
	FILE* file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr, "Failed to open shader file %s\n", filename);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buffer = (char*)malloc(length + 1);
	if (!buffer) {
		fprintf(stderr, "Failed to allocate memory for shader source\n");
		fclose(file);
		return NULL;
	}

	fread(buffer, 1, length, file);
	buffer[length] = '\0';
	fclose(file);
	return buffer;
}

static GLuint compile_shader(GLenum shaderType, const char* shaderSource) {
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLint max_length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);
		char* info_log = (char*)malloc(max_length);
		glGetShaderInfoLog(shader, max_length, NULL, info_log);
		fprintf(stderr, "Shader compilation failed: %s\n", info_log);
		free(info_log);
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

static GLuint create_shader_program(GLuint vertex_shader, GLuint fragment_shader) {
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		GLint max_length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);
		char* info_log = (char*)malloc(max_length);
		glGetProgramInfoLog(program, max_length, NULL, info_log);
		fprintf(stderr, "Shader program linking failed: %s\n", info_log);
		free(info_log);
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		glDeleteProgram(program);
		return -1;
	}

	return program;
}

static GLuint shader_from_file(const char* path, GLenum shader_type) {
	char* shader_source = read_entire_file(path);
	if (shader_source == NULL) {
		return -1;
	}
	GLuint shader = compile_shader(shader_type, shader_source);
	free(shader_source);
	return shader;
}

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}

int main() {
	assert(SDL_Init(SDL_INIT_VIDEO) == 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Window* window = SDL_CreateWindow("Raymarcher", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
	assert(window != NULL);

	SDL_GLContext context = SDL_GL_CreateContext(window);
	assert(context != NULL);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		printf("Failed to initialize Glad\n");
		return -1;
	}

	SDL_GL_MakeCurrent(window, context);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	GLuint vertex_shader = shader_from_file("vertex_shader.glsl", GL_VERTEX_SHADER);
	GLuint fragment_shader = shader_from_file("fragment_shader.glsl", GL_FRAGMENT_SHADER);
	GLuint shader_program = create_shader_program(vertex_shader, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	glUseProgram(shader_program);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	SDL_Event event;
	while (1) {
		while (SDL_PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_QUIT:
				goto exit;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_r) {
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
					}
				}
			}
		}

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		SDL_GL_SwapWindow(window);
	}

exit:
	glDeleteProgram(shader_program);
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
