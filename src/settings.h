#ifndef _ETHER_SETTINGS_H
#define _ETHER_SETTINGS_H

#include "utils.h"

#include "SDL3/SDL.h"

#define WINDOW_TITLE "etheral-rays (ETHER)"
#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 720

#define RENDER_RATIO 1

#ifndef RENDER_RATIO
    #define RENDER_WIDTH 320
    #define RENDER_HEIGHT 240
#else
    #define RENDER_WIDTH (WINDOW_WIDTH / RENDER_RATIO)
    #define RENDER_HEIGHT (WINDOW_HEIGHT / RENDER_RATIO)
#endif

#define BIND(bind, code) STATE_DEFAULT_KEYBINDS_##bind = SDL_SCANCODE_##code ,

enum STATE_DEFAULT_KEYBINDS
{
    BIND(MOVE_UP, W)
    BIND(MOVE_DOWN, S)
    BIND(MOVE_LEFT, A)
    BIND(MOVE_RIGHT, D)
    BIND(SMTH, SPACE)
};

// TODO: just fix the fucking collisions

#endif
