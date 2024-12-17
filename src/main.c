#include "SDL3/SDL.h"

#include "utils.h"
#include "settings.h"
#include "state.h"
#include "renderer.h"

void handle_event(SDL_Event *event, BOOL *quit, ETHER_state_input *input, ETHER_state_keybinds *keybinds);

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *sdl_window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS);
    SDL_Renderer *sdl_renderer = SDL_CreateRenderer(sdl_window, NULL);
    SDL_SetRenderLogicalPresentation(sdl_renderer, RENDER_WIDTH, RENDER_HEIGHT, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    ETHER_state_input state_input = {};
    ETHER_state_keybinds state_keybinds = create_default_state_keybinds();

    ETHER_state state;
    state.input = &state_input;
    state.keybinds = &state_keybinds;

    BOOL quit = FALSE;
    while (!quit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            handle_event(&event, &quit, state.input, state.keybinds);
        }

        ETHER_render(sdl_renderer, &state);
    }

    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(sdl_window);

    SDL_Quit();
}

#define HANDLE_BOUND_INPUT(bind) \
    if (code == keybinds->bind) \
        input->bind = (event->type == SDL_EVENT_KEY_DOWN); \

void handle_event(SDL_Event *event, BOOL *quit, ETHER_state_input *input, ETHER_state_keybinds *keybinds)
{
    if (event->type == SDL_EVENT_QUIT)
    {
        *quit = TRUE;
    }
    else if (event->type == SDL_EVENT_KEY_DOWN || SDL_EVENT_KEY_UP)
    {
        SDL_Scancode code = event->key.scancode;
        HANDLE_BOUND_INPUT(move_up)
        HANDLE_BOUND_INPUT(move_down)
        HANDLE_BOUND_INPUT(move_right)
        HANDLE_BOUND_INPUT(move_left)
    }
}