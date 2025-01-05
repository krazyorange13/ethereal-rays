#ifndef ETHER_CONTROLLER_H
#define ETHER_CONTROLLER_H

#include "state.h"

void ETHER_update(ETHER_state *state);
void ETHER_handle_event(SDL_Event *event, ETHER_state *state);
void ETHER_handle_entities(ETHER_state *state);
void ETHER_handle_entity_buckets(ETHER_state_quadtree *quadtree, ETHER_entity *entity, ETHER_rect_u16 entity_rect_old);
void ETHER_handle_entity_collisions(ETHER_entity *entity);
void ETHER_entities_depth_sort(ETHER_state_entities *entities);
void ETHER_entities_debug(ETHER_state_entities *entities, ETHER_entity *marker0, ETHER_entity *marker1, ETHER_entity *marker2);

#endif
