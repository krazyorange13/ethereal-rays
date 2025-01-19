#ifndef ETHER_CONTROLLER_H
#define ETHER_CONTROLLER_H

#include "state.h"

void ETHER_update(ETHER_state *state);
void ETHER_handle_mouse(ETHER_state *state);
void ETHER_handle_events(ETHER_state *state);
void ETHER_handle_event(SDL_Event *event, ETHER_state *state);
void ETHER_handle_entities(ETHER_state *state);
void ETHER_handle_entities_movement(ETHER_state *state);
void ETHER_handle_entities_depth_sort(ETHER_state *state);
void ETHER_handle_entities_chunks(ETHER_state *state);
void ETHER_handle_entities_collisions(ETHER_state *state);

#endif
