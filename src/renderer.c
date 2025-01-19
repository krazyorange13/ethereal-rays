#include "renderer.h"
#include "settings.h"

#include "string.h"

void ETHER_render(ETHER_state *state)
{
    SDL_SetRenderDrawColor(state->sdl_renderer, 250, 250, 250, 255);
    SDL_RenderClear(state->sdl_renderer);

    ETHER_render_chunks(state);
    ETHER_render_entities(state);
    ETHER_render_debug(state);

    SDL_RenderPresent(state->sdl_renderer);
}

void ETHER_render_chunks(ETHER_state *state)
{
    ETHER_block_chunks *chunks = state->chunks;
    BOOL *render_cache_flags = chunks->render_cache_flags;
    SDL_Texture **render_caches = chunks->render_caches;

    for (ETHER_chunk_id_t i = 0; i < chunks->len; i++)
    {
        if (render_cache_flags[i] == FLAG_DIRTY)
        {
            if (render_caches[i] == NULL)
            {
                SDL_Texture *cache = SDL_CreateTexture(state->sdl_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, ETHER_BLOCK_CHUNK_WORLD_SIZE, ETHER_BLOCK_CHUNK_WORLD_SIZE);
                SDL_SetTextureScaleMode(cache, SDL_SCALEMODE_NEAREST);
                render_caches[i] = cache;
            }
            render_cache_flags[i] = FLAG_CLEAN;
            SDL_SetRenderTarget(state->sdl_renderer, render_caches[i]);
            for (uint16_t j = 0; j < SQUARE(ETHER_BLOCK_CHUNK_SIZE); j++)
            {
                SDL_Texture *tex = ETHER_blockid_to_texture(state->textures, chunks->chunks[i].blocks[j].tex);
                SDL_FRect frect;
                frect.x = (j % ETHER_BLOCK_CHUNK_SIZE) * ETHER_BLOCK_SIZE;
                frect.y = (j / ETHER_BLOCK_CHUNK_SIZE) * ETHER_BLOCK_SIZE;
                frect.w = ETHER_BLOCK_SIZE;
                frect.h = ETHER_BLOCK_SIZE;
                SDL_RenderTexture(state->sdl_renderer, tex, NULL, &frect);
            }
            SDL_SetRenderTarget(state->sdl_renderer, NULL);
        }
        ETHER_rect_block_chunk_space rect = chunks->rects[i];
        SDL_FRect frect;
        frect.x = rect.x * ETHER_BLOCK_CHUNK_WORLD_SIZE;
        frect.y = rect.y * ETHER_BLOCK_CHUNK_WORLD_SIZE;
        frect.w = ETHER_BLOCK_CHUNK_WORLD_SIZE;
        frect.h = ETHER_BLOCK_CHUNK_WORLD_SIZE;
        SDL_RenderTexture(state->sdl_renderer, chunks->render_caches[i], NULL, &frect);
    }
}

#include "float.h"

void ETHER_render_entities(ETHER_state *state)
{
    for (ETHER_entity_id_t i = 0; i < state->entities->len; i++)
    {
        SDL_FRect frect = ETHER_rect_u16_to_sdl(state->entities->rects[i]);
        // SDL_SetRenderDrawColor(state->sdl_renderer, 255, 0, 0, 100);
        // SDL_SetRenderDrawBlendMode(state->sdl_renderer, SDL_BLENDMODE_BLEND);
        // SDL_RenderFillRect(state->sdl_renderer, &frect);
        SDL_RenderTexture(state->sdl_renderer, state->textures->item, NULL, &frect);
    }
}

#define DEBUG_INPUT_BINDING(bind, row) \
    char *__##bind; \
    char *__##bind##_val; \
    size_t __##bind##_len = strlen(#bind) + 15; \
    __##bind = malloc(__##bind##_len); /* +15 (padding) +1 (value) +1 (null) */ \
    __##bind##_val = malloc(2); \
    snprintf(__##bind##_val, 2, "%d", state->input->bind); \
    snprintf(__##bind, __##bind##_len, "%-15s%s", #bind, __##bind##_val); \
    SDL_RenderDebugText(state->sdl_renderer, 10, row * 10, __##bind);

void ETHER_render_debug(ETHER_state *state)
{
    SDL_Renderer *renderer = state->sdl_renderer;

    for (ETHER_entity_chunk_id_t i = 0; i < state->entities->chunks->len; i++)
    {
        ETHER_rect_entity_chunk_space erect = state->entities->chunks->rects[i];
        SDL_FRect frect;
        frect.x = erect.x * ETHER_ENTITY_CHUNK_SIZE_WORLD;
        frect.y = erect.y * ETHER_ENTITY_CHUNK_SIZE_WORLD;
        frect.w = erect.w * ETHER_ENTITY_CHUNK_SIZE_WORLD;
        frect.h = erect.h * ETHER_ENTITY_CHUNK_SIZE_WORLD;

        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 100);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderRect(renderer, &frect);

        for (ETHER_entity_chunk_cell_id_t j = 0; j < SQUARE(ETHER_ENTITY_CHUNK_SIZE_CELLS); j++)
        {
            SDL_FRect _frect = frect;
            _frect.x += (j % ETHER_ENTITY_CHUNK_SIZE_CELLS) * ETHER_ENTITY_CHUNK_CELL_SIZE;
            _frect.y += (j / ETHER_ENTITY_CHUNK_SIZE_CELLS) * ETHER_ENTITY_CHUNK_CELL_SIZE;
            _frect.w = ETHER_ENTITY_CHUNK_CELL_SIZE;
            _frect.h = ETHER_ENTITY_CHUNK_CELL_SIZE;

            SDL_SetRenderDrawColor(renderer, 255, 0, 255, 100);
            // SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_RenderRect(renderer, &_frect);

            uint16_t _x = _frect.x + _frect.w / 2;
            uint16_t _y = _frect.y + _frect.h / 2;
            char _str[4];
            snprintf(_str, 4, "%u", state->entities->chunks->chunks[i].cells[j].len);
            SDL_SetRenderDrawColor(renderer, 255, 0, 255, 200);
            SDL_RenderDebugText(renderer, _x, _y, _str);
        }
    }


    // SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    // DEBUG_INPUT_BINDING(move_up, 1)
    // DEBUG_INPUT_BINDING(move_down, 2)
    // DEBUG_INPUT_BINDING(move_left, 3)
    // DEBUG_INPUT_BINDING(move_right, 4)

    char __fps[6];
    snprintf(__fps, 6, "%3.3lf", state->fps);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, 9, 9, __fps);
    SDL_RenderDebugText(renderer, 9, 10, __fps);
    SDL_RenderDebugText(renderer, 9, 11, __fps);
    SDL_RenderDebugText(renderer, 11, 11, __fps);
    SDL_RenderDebugText(renderer, 11, 10, __fps);
    SDL_RenderDebugText(renderer, 11, 9, __fps);
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderDebugText(renderer, 10, 10, __fps);
}
