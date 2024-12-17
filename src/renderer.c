#include "renderer.h"

#include "string.h"

void ETHER_render_debug(SDL_Renderer *renderer, ETHER_state *state);

void ETHER_render(SDL_Renderer *renderer, ETHER_state *state)
{
    SDL_SetRenderDrawColor(renderer, 250, 250, 250, 255);
    SDL_RenderClear(renderer);

    ETHER_render_debug(renderer, state);

    SDL_RenderPresent(renderer);
}

#define DEBUG_INPUT_BINDING(bind, row) \
    char *__##bind; \
    char *__##bind##_val; \
    size_t __##bind##_len = strlen(#bind) + 15; \
    __##bind = malloc(__##bind##_len); /* +15 (padding) +1 (value) +1 (null) */ \
    __##bind##_val = malloc(2); \
    snprintf(__##bind##_val, 2, "%d", state->input->bind); \
    snprintf(__##bind, __##bind##_len, "%-15s%s", #bind, __##bind##_val); \
    SDL_RenderDebugText(renderer, 10, row * 10, __##bind);

void ETHER_render_debug(SDL_Renderer *renderer, ETHER_state *state)
{
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    DEBUG_INPUT_BINDING(move_up, 1)
    DEBUG_INPUT_BINDING(move_down, 2)
    DEBUG_INPUT_BINDING(move_left, 3)
    DEBUG_INPUT_BINDING(move_right, 4)
}