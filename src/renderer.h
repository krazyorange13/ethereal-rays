#ifndef _ETHER_RENDER_H
#define _ETHER_RENDER_H

#include "utils.h"
#include "state.h"

#include "SDL3/SDL.h"

typedef struct _ETHER_renderer ETHER_renderer;

struct _ETHER_renderer
{

};

void ETHER_render(SDL_Renderer *renderer, ETHER_state *state);

#endif