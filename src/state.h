#ifndef _ETHER_STATE_H
#define _ETHER_STATE_H

#include "utils.h"

#include "SDL3/SDL.h"

typedef struct _ETHER_state ETHER_state;
typedef struct _ETHER_state_input ETHER_state_input;
typedef struct _ETHER_state_keybinds ETHER_state_keybinds;
typedef struct _ETHER_state_entities ETHER_state_entities;
typedef struct _ETHER_state_textures ETHER_state_textures;

typedef struct _ETHER_entity ETHER_entity;

ETHER_state_keybinds create_default_state_keybinds();

struct _ETHER_state
{
    double fps;
    BOOL quit;
    ETHER_state_input *input;
    ETHER_state_keybinds *keybinds;
    ETHER_state_textures *textures;
    ETHER_state_entities *entities;
};

struct _ETHER_state_input
{
    bool move_up;
    bool move_down;
    bool move_left;
    bool move_right;
};

struct _ETHER_state_keybinds
{
    SDL_Scancode move_up;
    SDL_Scancode move_down;
    SDL_Scancode move_right;
    SDL_Scancode move_left;
};

struct _ETHER_state_entities
{
    uint16_t count;
    ETHER_entity *head;
    ETHER_entity *tail;
};

struct _ETHER_entity
{
    ETHER_vec2 pos;
    SDL_Texture *texture;
    struct _ETHER_entity *next;
    struct _ETHER_entity *prev;
};

struct _ETHER_state_textures
{
    SDL_Texture *player;
    SDL_Texture *gem;
};

void ETHER_state_textures_load(SDL_Renderer *renderer, ETHER_state_textures *textures);

#endif