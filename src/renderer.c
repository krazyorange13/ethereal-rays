#include "renderer.h"
#include "settings.h"

#include "string.h"

void ETHER_render_leaf(SDL_Renderer *renderer, ETHER_state_textures *textures, ETHER_leaf *leaf);
void ETHER_render_debug(SDL_Renderer *renderer, ETHER_state *state);

void ETHER_render(ETHER_state *state)
{
    SDL_SetRenderDrawColor(state->sdl_renderer, 250, 250, 250, 255);
    SDL_RenderClear(state->sdl_renderer);

    ETHER_rect_u8 quadtree_viewport = ETHER_rect_world_to_quadtree((ETHER_rect_u16) { 0, 0, RENDER_WIDTH, RENDER_HEIGHT });
    ETHER_array *leaves = ETHER_node_get_rect_leaves(state->quadtree->base, quadtree_viewport);
    for (uint16_t i = 0; i < leaves->len; i++)
    {
        ETHER_leaf *leaf = leaves->data[i];
        if (state->update_textures)
        {
            leaf->chunk.update_cache = TRUE;
            for (uint8_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
            {
                leaf->chunk.blocks[i] = (leaf->chunk.blocks[i] + 1) % 20;
            }
        }
        ETHER_render_leaf(state->sdl_renderer, state->textures, leaf);
    }
    state->update_textures = FALSE;

    ETHER_array_destroy(leaves);

    for (ETHER_entity *entity = state->entities->head; entity != NULL; entity = entity->next)
    {
        SDL_FRect frect = ETHER_rect_u16_to_sdl(entity->rect);
        SDL_SetRenderDrawColor(state->sdl_renderer, 255, 0, 0, 255);
        SDL_RenderRect(state->sdl_renderer, &frect);
        SDL_RenderTexture(state->sdl_renderer, entity->tex, NULL, &frect);
    }

    // SDL_SetRenderDrawColor(state->sdl_renderer, 0, 0, 255, 100);
    // SDL_SetRenderDrawBlendMode(state->sdl_renderer, SDL_BLENDMODE_BLEND);
    // SDL_FRect frect = ETHER_rect_u16_to_sdl(ETHER_rect_quadtree_to_world(ETHER_rect_world_to_quadtree_2(state->player->rect)));
    // SDL_RenderFillRect(state->sdl_renderer, &frect);

    ETHER_render_debug(state->sdl_renderer, state);

    SDL_RenderPresent(state->sdl_renderer);
}

void ETHER_render_leaf(SDL_Renderer *renderer, ETHER_state_textures *textures, ETHER_leaf *leaf)
{
    if (leaf->chunk.cache == NULL || leaf->chunk.update_cache)
    {
        if (leaf->chunk.cache == NULL)
        {
            // TODO: review PIXELFORMAT and TEXTUREACCESS
            SDL_Texture *cache = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, CHUNK_WORLD_SIZE, CHUNK_WORLD_SIZE);
            SDL_SetTextureScaleMode(cache, SDL_SCALEMODE_NEAREST);
            leaf->chunk.cache = cache;
        }
        leaf->chunk.update_cache = FALSE;
        SDL_SetRenderTarget(renderer, leaf->chunk.cache);
        for (uint8_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
        {
            SDL_Texture *tex = ETHER_blockid_to_texture(textures, leaf->chunk.blocks[i]);
            SDL_FRect frect;// = ETHER_rect_u16_to_sdl(leaf->rect_world);
            frect.x = (i % 8) * BLOCK_SIZE;
            frect.y = (i / 8) * BLOCK_SIZE;
            frect.w = BLOCK_SIZE;
            frect.h = BLOCK_SIZE;
            SDL_RenderTexture(renderer, tex, NULL, &frect);
        }
        SDL_SetRenderTarget(renderer, NULL);
    }

    SDL_FRect frect = ETHER_rect_u16_to_sdl(leaf->rect_world);
    SDL_RenderTexture(renderer, leaf->chunk.cache, NULL, &frect);

    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
    SDL_RenderRect(renderer, &frect);

    char *ptr_str = malloc(12);
    snprintf(ptr_str, 12, "%p", &leaf->bucket);
    SDL_RenderDebugText(renderer, leaf->rect_world.x + 32, leaf->rect_world.y + 32, ptr_str);
    free(ptr_str);
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

void ETHER_render_node_debug(SDL_Renderer *renderer, ETHER_node *node, uint16_t mul)
{
    if (node == NULL)
        return;

    SDL_FRect sdl_frect = {
        node->rect.x * mul,
        node->rect.y * mul,
        node->rect.w * mul,
        node->rect.h * mul
    };

    if (!node->is_leaf)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderRect(renderer, &sdl_frect);
    }
    else
    {
        sdl_frect.x += 1;
        sdl_frect.y += 1;
        sdl_frect.w += mul;
        sdl_frect.h += mul;
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &sdl_frect);
        return;
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        ETHER_render_node_debug(renderer, node->branch.quad[i], mul);
    }
}

void ETHER_render_debug(SDL_Renderer *renderer, ETHER_state *state)
{
    // SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    // DEBUG_INPUT_BINDING(move_up, 1)
    // DEBUG_INPUT_BINDING(move_down, 2)
    // DEBUG_INPUT_BINDING(move_left, 3)
    // DEBUG_INPUT_BINDING(move_right, 4)

    // ETHER_render_node_debug(renderer, state->quadtree->base, state->mouse.x / 16);
    // ETHER_render_node_debug(renderer, state->quadtree->base, CHUNK_WORLD_SIZE);

    char *__fps = malloc(6);
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
