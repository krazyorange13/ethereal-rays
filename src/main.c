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

    SDL_Window *sdl_window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS);
    SDL_Renderer *sdl_renderer = SDL_CreateRenderer(sdl_window, NULL);
    SDL_SetRenderVSync(sdl_renderer, 1);
    SDL_SetRenderLogicalPresentation(sdl_renderer, RENDER_WIDTH, RENDER_HEIGHT, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    ETHER_state_input state_input = {};
    ETHER_state_keybinds state_keybinds = create_default_state_keybinds();

    ETHER_state_textures state_textures = {};
    ETHER_state_textures_load(sdl_renderer, &state_textures);

    ETHER_state_entities state_entities = {};

    // #define ENTITY_COUNT 1000

    // srand(time(NULL));
    // ETHER_entity *temp_entities = malloc(sizeof(ETHER_entity) * ENTITY_COUNT);
    // for (int i = 0; i < ENTITY_COUNT; i++)
    // {
    //     ETHER_entity *temp_entity = temp_entities + i;
    //     temp_entity->texture = state_textures.gem;
    //     temp_entity->pos.x = rand() % RENDER_WIDTH;
    //     temp_entity->pos.y = rand() % RENDER_HEIGHT;
    //     ETHER_entities_add(&state_entities, temp_entity);
    // }

    ETHER_state_quadtree state_quadtree;
    ETHER_node_create(&state_quadtree.base, NULL, 0);
    ETHER_node_subdivide(state_quadtree.base);
    ETHER_node_subdivide(state_quadtree.base->quad[0].node);
    ETHER_node_subdivide(state_quadtree.base->quad[0].node->quad[3].node);
    ETHER_node_debug(state_quadtree.base);

    ETHER_state state;
    state.fps = 0;
    state.quit = FALSE;
    state.input = &state_input;
    state.keybinds = &state_keybinds;
    state.textures = &state_textures;
    state.entities = &state_entities;
    state.quadtree = &state_quadtree;

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

        ETHER_render(sdl_renderer, &state);

        prev_time = this_time;
        dt4 = dt3; dt3 = dt2; dt2 = dt1; dt1 = delta_time;
    }

    // TODO causes crash right now
    // ETHER_state_entities_free(&state_entities);

    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(sdl_window);

    SDL_Quit();

    return 0;
}