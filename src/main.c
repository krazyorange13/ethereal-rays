#include "time.h"

#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"

#include "utils.h"
#include "settings.h"
#include "renderer.h"
#include "controller.h"

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *sdl_window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    SDL_Renderer *sdl_renderer = SDL_CreateRenderer(sdl_window, NULL);
    SDL_SetRenderVSync(sdl_renderer, 1);
    SDL_SetRenderLogicalPresentation(sdl_renderer, RENDER_WIDTH, RENDER_HEIGHT, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    ETHER_state_input state_input = {};
    ETHER_state_keybinds state_keybinds = ETHER_keybinds_get_default();

    ETHER_state_chunks state_chunks;
    state_chunks.len = 1024;
    state_chunks.cap = 1024;
    state_chunks.rects = malloc(state_chunks.cap * sizeof(*state_chunks.rects));
    state_chunks.chunks = malloc(state_chunks.cap * sizeof(*state_chunks.chunks));
    state_chunks.render_cache_flags = malloc(state_chunks.cap * sizeof(*state_chunks.render_cache_flags));
    state_chunks.render_caches = malloc(state_chunks.cap * sizeof(*state_chunks.render_caches));

    for (ETHER_chunk_id_t i = 0; i < state_chunks.len; i++)
    {
        ETHER_rect_block_chunk_space rect;
        rect.x = i % 32;
        rect.y = i / 32;
        rect.w = 1;
        rect.h = 1;
        state_chunks.rects[i] = rect;
        state_chunks.render_cache_flags[i] = FLAG_DIRTY;
        state_chunks.render_caches[i] = NULL;

        ETHER_block_chunk chunk;
        for (uint16_t i = 0; i < SQUARE(ETHER_BLOCK_CHUNK_SIZE); i++)
        {
            chunk.blocks[i].tex = ETHER_TEXTURE_BLOCK_BRICKS_STONE;//rand() % 7 + 1;
        }
        state_chunks.chunks[i] = chunk;
    }

    ETHER_state_entities state_entities;
    state_entities.len = 2000;
    state_entities.cap = 2000;
    state_entities.rects = malloc(state_entities.cap * sizeof(*state_entities.rects));
    state_entities.velocities = malloc(state_entities.cap * sizeof(*state_entities.velocities));
    state_entities.types = malloc(state_entities.cap * sizeof(*state_entities.types));
    state_entities.states = malloc(state_entities.cap * sizeof(*state_entities.states));

    // printf("%ld\n", sizeof(ETHER_entity_type_t));
    // printf("%ld\n", sizeof(ETHER_entity_behavior_t));
    // printf("%ld\n", sizeof(ETHER_entity_direction_t));
    // printf("%ld\n", sizeof(ETHER_entity_state));
    // printf("%ld\n", sizeof(ETHER_animation));

    srand(time(NULL));
    for (ETHER_entity_id_t i = 0; i < state_entities.len; i++)
    {
        ETHER_rect_world_space rect;
        rect.x = (rand() % (ETHER_ENTITY_CHUNK_SIZE_WORLD * 2)) + ETHER_ENTITY_CHUNK_SIZE_WORLD;
        rect.y = (rand() % (ETHER_ENTITY_CHUNK_SIZE_WORLD * 2)) + ETHER_ENTITY_CHUNK_SIZE_WORLD;
        rect.w = ETHER_ENTITY_SIZE;
        rect.h = ETHER_ENTITY_SIZE;
        state_entities.rects[i] = rect;
        state_entities.velocities[i] = (ETHER_rect_s16) {0, 0, 0, 0};
        ETHER_entity_type_t type = ETHER_ENTITY_TYPE_PERSON;
        state_entities.types[i] = type;
        ETHER_entity_state state = {};
        state.behavior = ETHER_ENTITY_BEHAVIOR_PERSON_IDLE; // initial state
        state.direction = ETHER_ENTITY_DIRECTION_LEFT;
        state_entities.states[i] = state;
    }

    state_entities.types[0] = ETHER_ENTITY_TYPE_PLAYER;

    state_entities.chunks = malloc(sizeof(*state_entities.chunks));
    state_entities.chunks->len = 256;
    state_entities.chunks->cap = 256;
    state_entities.chunks->rects = malloc(sizeof(*state_entities.chunks->rects) * state_entities.chunks->cap);
    state_entities.chunks->chunks = malloc(sizeof(*state_entities.chunks->chunks) * state_entities.chunks->cap);
    memset(state_entities.chunks->chunks, 0, sizeof(*state_entities.chunks->chunks) * state_entities.chunks->cap);

    for (ETHER_entity_chunk_id_t i = 0; i < state_entities.chunks->len; i++)
    {
        ETHER_rect_entity_chunk_space rect;
        rect.x = i % 16;
        rect.y = i / 16;
        rect.w = 1;
        rect.h = 1;
        state_entities.chunks->rects[i] = rect;
    }

    ETHER_state_textures state_textures = {};
    state_textures.block = IMG_LoadTexture(sdl_renderer, ETHER_TEXTURE_SHEET_PATH_BLOCK);
    state_textures.entity = IMG_LoadTexture(sdl_renderer, ETHER_TEXTURE_SHEET_PATH_ENTITY);
    state_textures.item = NULL;
    state_textures.effect = NULL;
    state_textures.ui = NULL;

    SDL_SetTextureScaleMode(state_textures.block, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(state_textures.entity, SDL_SCALEMODE_NEAREST);

    ETHER_state_animations state_animations = {};
    state_animations.person_idle = (ETHER_animation) {ETHER_TEXTURE_ENTITY_PERSON_IDLE_0, 4, 6, ETHER_TEXTURE_SHEET_ENTITY};
    state_animations.person_walk = (ETHER_animation) {ETHER_TEXTURE_ENTITY_PERSON_WALK_0, 4, 6, ETHER_TEXTURE_SHEET_ENTITY};

    ETHER_state state;
    state.sdl_window = sdl_window;
    state.sdl_renderer = sdl_renderer;
    state.input = &state_input;
    state.keybinds = &state_keybinds;
    state.entities = &state_entities;
    state.chunks = &state_chunks;
    state.textures = &state_textures;
    state.animations = &state_animations;
    state.fps = 0;
    state.quit = FALSE;
    state.smth = FALSE;
    state.collision_cycle = 0;
    state.frames = 0;
    state.camera = (ETHER_rect_s16) {0, 0, RENDER_WIDTH, RENDER_HEIGHT};

    uint64_t frequency = SDL_GetPerformanceFrequency();
    uint64_t frame_timer = 0;
    uint64_t this_time = SDL_GetPerformanceCounter();
    uint64_t prev_time = SDL_GetPerformanceCounter();
    double delta_time;
    double dt1 = 0, dt2 = 0, dt3 = 0, dt4 = 0;

    while (!state.quit)
    {
        this_time = SDL_GetPerformanceCounter();
        delta_time = ((double) this_time - (double) prev_time) / frequency * 1000.0;

        if (this_time > frame_timer + frequency)
        {
            frame_timer = this_time;
            state.fps = 1000.0 / ((delta_time + dt1 + dt2 + dt3 + dt4) / 5);
        }

        ETHER_update(&state);

        ETHER_render(&state);

        prev_time = this_time;
        dt4 = dt3; dt3 = dt2; dt2 = dt1; dt1 = delta_time;
        state.frames++;
    }

    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(sdl_window);

    SDL_Quit();

    return 0;
}
