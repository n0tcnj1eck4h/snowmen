#include "utils.h"
#include <string.h>

char* read_entire_file(const char* filename) {
	FILE* file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr, "Failed to open shader file %s\n", filename);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buffer = (char*)malloc(length + 1);
	memset(buffer, 0, length);
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

GLuint compile_shader(GLenum shaderType, const char* shaderSource) {
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

GLuint create_shader_program(GLuint vertex_shader, GLuint fragment_shader) {
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
		return 0;
	}

	return program;
}

GLuint shader_from_file(const char* path, GLenum shader_type) {
	char* shader_source = read_entire_file(path);
	if (shader_source == NULL) {
		return -1;
	}
	GLuint shader = compile_shader(shader_type, shader_source);
	free(shader_source);
	return shader;
}

void GLAPIENTRY gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}
