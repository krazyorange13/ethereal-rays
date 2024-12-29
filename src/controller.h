#ifndef ETHER_CONTROLLER_H
#define ETHER_CONTROLLER_H

#include "state.h"

void ETHER_update(ETHER_state *state);

void ETHER_entities_add(ETHER_state_entities *entities, ETHER_entity *entity);
ETHER_entity *ETHER_entities_pop(ETHER_state_entities *entities);

#endif