#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "cglm/types.h"
#include <stdbool.h>

typedef struct {
	vec3 camera_direction;
	vec3 camera_position;
	bool w_held;
	bool s_held;
	bool a_held;
	bool d_held;
} GameState;

#endif
