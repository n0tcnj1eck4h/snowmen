#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_stdinc.h>
#include <SDL_video.h>
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <stdbool.h>

char* read_entire_file(const char* filename);
GLuint compile_shader(GLenum shaderType, const char* shaderSource);
GLuint create_shader_program(GLuint vertex_shader, GLuint fragment_shader);
GLuint shader_from_file(const char* path, GLenum shader_type);
void GLAPIENTRY gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

#endif
