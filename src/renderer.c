#include "renderer.h"
#include "settings.h"

#include "string.h"

void ETHER_render_debug(SDL_Renderer *renderer, ETHER_state *state);

void ETHER_render(SDL_Renderer *renderer, ETHER_state *state)
{
    SDL_SetRenderDrawColor(renderer, 250, 250, 250, 255);
    SDL_RenderClear(renderer);

    for (ETHER_entity *entity = state->entities->head; entity != NULL; entity = entity->next)
    {
        SDL_FRect rect;
        rect.x = entity->pos.x;
        rect.y = entity->pos.y;
        rect.w = entity->texture->w;
        rect.h = entity->texture->h;
        SDL_RenderTexture(renderer, entity->texture, NULL, &rect);
    }

    // SDL_FRect player_rect;
    // player_rect.w = state->textures->player->w;
    // player_rect.h = state->textures->player->h;
    // player_rect.x = (RENDER_WIDTH - player_rect.w) / 2;
    // player_rect.y = (RENDER_HEIGHT - player_rect.h) / 2;
    // SDL_RenderTexture(renderer, state->textures->player, NULL, &player_rect);

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

void ETHER_render_node_debug(SDL_Renderer *renderer, ETHER_node *node)
{
    if (node == NULL)
        return;

    SDL_FRect sdl_frect = {
        node->rect.x,
        node->rect.y,
        node->rect.w,
        node->rect.h
    };
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderRect(renderer, &sdl_frect);

    for (uint8_t i = 0; i < 4; i++)
    {
        ETHER_render_node_debug(renderer, node->quad[i].node);
    }
}

void ETHER_render_debug(SDL_Renderer *renderer, ETHER_state *state)
{
    // SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    // DEBUG_INPUT_BINDING(move_up, 1)
    // DEBUG_INPUT_BINDING(move_down, 2)
    // DEBUG_INPUT_BINDING(move_left, 3)
    // DEBUG_INPUT_BINDING(move_right, 4)

    ETHER_render_node_debug(renderer, state->quadtree->base);

    char *__fps = malloc(6);
    snprintf(__fps, 6, "%3.3lf", state->fps);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, 9, 11, __fps);
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderDebugText(renderer, 10, 10, __fps);
}