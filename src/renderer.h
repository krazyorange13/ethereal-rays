#ifndef _ETHER_RENDERER_H
#define _ETHER_RENDERER_H

#include "utils.h"
#include "state.h"
#include "controller.h"

#include "SDL3/SDL.h"

void ETHER_render(ETHER_state *state);
void ETHER_render_chunks(ETHER_state *state);
void ETHER_render_entities(ETHER_state *state);
void ETHER_render_debug(ETHER_state *state);

#endif
