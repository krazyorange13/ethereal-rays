#ifndef ETHER_CONTROLLER_H
#define ETHER_CONTROLLER_H

#include "state.h"

void ETHER_update(ETHER_state *state);

void ETHER_entities_add(ETHER_state_entities *entities, ETHER_entity *entity);
ETHER_entity *ETHER_entities_pop(ETHER_state_entities *entities);

void ETHER_node_debug(ETHER_node *node);
void ETHER_node_create(ETHER_node **node, ETHER_node *parent);
void ETHER_node_subdivide(ETHER_node *node);
uint8_t ETHER_node_get_parent_pos(ETHER_node *node);
ETHER_rect ETHER_node_get_rect(ETHER_node *node);

#endif