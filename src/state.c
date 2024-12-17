#include "state.h"
#include "settings.h"

ETHER_state_keybinds create_default_state_keybinds()
{
    ETHER_state_keybinds state;
    state.move_up = STATE_DEFAULT_KEYBINDS_MOVE_UP;
    state.move_down = STATE_DEFAULT_KEYBINDS_MOVE_DOWN;
    state.move_left = STATE_DEFAULT_KEYBINDS_MOVE_LEFT;
    state.move_right = STATE_DEFAULT_KEYBINDS_MOVE_RIGHT;
    return state;
}

ETHER_state_entities ETHER_create_state_entities()
{
    ETHER_state_entities entities;
    entities.count = 0;
    entities.head = NULL;
    entities.tail = NULL;
    return entities;
}

void ETHER_state_entities_add(ETHER_state_entities *entities, ETHER_entity *entity)
{
    if (entities->count)
    {
        entities->tail->next = entity;
        entity->prev = entities->tail;
        entities->tail = entity;
        entity->next = NULL;
    }
    else
    {
        entities->head = entity;
        entities->tail = entity;
        entity->next = NULL;
        entity->prev = NULL;
    }
    entities->count++;
}

ETHER_entity *ETHER_state_entities_pop(ETHER_state_entities *entities)
{
    if (entities->count)
    {
        ETHER_entity *p = entities->tail;
        p->prev->next = NULL;
        p->prev = NULL;
        return p;
    }
    else
    {
        return NULL;
    }
}