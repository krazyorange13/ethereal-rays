#ifndef _ETHER_STATE_H
#define _ETHER_STATE_H

#include "utils.h"

#include "SDL3/SDL.h"

typedef struct _ETHER_state ETHER_state;
typedef struct _ETHER_state_input ETHER_state_input;
typedef struct _ETHER_state_keybinds ETHER_state_keybinds;

ETHER_state_keybinds create_default_state_keybinds();

struct _ETHER_state
{
    ETHER_state_input *input;
    ETHER_state_keybinds *keybinds;
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

#endif