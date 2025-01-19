#include "controller.h"

#include "settings.h"

void ETHER_update(ETHER_state *state)
{
    ETHER_handle_events(state);
    ETHER_handle_mouse(state);
    ETHER_handle_entities(state);
}

void ETHER_handle_mouse(ETHER_state *state)
{
    SDL_GetMouseState(&state->mouse.x, &state->mouse.y);
}

void ETHER_handle_events(ETHER_state *state)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ETHER_handle_event(&event, state);
    }
}

#define HANDLE_BOUND_INPUT(bind) \
    if (code == state->keybinds->bind) \
        state->input->bind = (event->type == SDL_EVENT_KEY_DOWN); \

void ETHER_handle_event(SDL_Event *event, ETHER_state *state)
{
    if (event->type == SDL_EVENT_QUIT)
    {
        state->quit = TRUE;
    }
    else if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP)
    {
        if (event->key.scancode == SDL_SCANCODE_Q)
            state->quit = TRUE;

        if (event->key.scancode == SDL_SCANCODE_SPACE)
            state->update_textures = TRUE;

        SDL_Scancode code = event->key.scancode;
        HANDLE_BOUND_INPUT(move_up)
        HANDLE_BOUND_INPUT(move_down)
        HANDLE_BOUND_INPUT(move_right)
        HANDLE_BOUND_INPUT(move_left)
    }
    else if (event->type == SDL_EVENT_WINDOW_RESIZED)
    {
        int w, h;
        SDL_GetWindowSize(state->sdl_window, &w, &h);
        SDL_SetRenderLogicalPresentation(state->sdl_renderer, w / RENDER_RATIO, h / RENDER_RATIO, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    }
}

void ETHER_handle_entities(ETHER_state *state)
{
    ETHER_handle_entities_movement(state);
    ETHER_handle_entities_depth_sort(state);
    ETHER_handle_entities_chunks(state);
    ETHER_handle_entities_collisions(state);
}

void ETHER_handle_entities_movement(ETHER_state *state)
{
    for (ETHER_entity_id_t i = 0; i < state->entities->len; i++)
    {
        // TODO: for entity movement to work properly it has to go
        // hand in hand with entity collisions. Refactor collision function
        // to suit this better.

        // state->entities->rects[i].x += (rand() % 3) - 1;
        // state->entities->rects[i].y += (rand() % 3) - 1;
        // state->entities->rects[i].x += SIGN2(RENDER_WIDTH / 2 - state->entities->rects[i].x);
        // state->entities->rects[i].y += SIGN2(RENDER_HEIGHT / 2 - state->entities->rects[i].y);
    }
}

void ETHER_handle_entities_depth_sort(ETHER_state *state)
{
    ETHER_state_entities *entities = state->entities;

    if (entities->len == 0 || entities->len == 1)
        return;

    for (ETHER_entity_id_t i = 1; i < entities->len; i++)
    {
        if (entities->rects[i].y >= entities->rects[i - 1].y)
            continue;

        ETHER_rect_world_space rect = entities->rects[i];
        ETHER_entity_id_t j = i;

        while(j != 0 && rect.y < entities->rects[j - 1].y)
        {
            entities->rects[j] = entities->rects[j - 1];
            j--;
        }

        entities->rects[j] = rect;
    }
}

typedef struct
{
    uint8_t len;
    uint8_t cap;
    ETHER_entity_chunk_cell **cells;
} ETHER_entity_chunk_cell_array;

ETHER_entity_chunk_cell_array ETHER_get_entity_chunk_cells(ETHER_state *state, ETHER_entity_id_t entity)
{
    ETHER_entity_chunk_cell_array array;
    array.len = 0;
    array.cap = 4;
    array.cells = malloc(sizeof(*array.cells) * array.cap);

    ETHER_entity_id_t i = entity;

    ETHER_rect_world_space rect = state->entities->rects[i];

    uint16_t cell_x1 = rect.x / ETHER_ENTITY_CHUNK_CELL_SIZE;
    uint16_t cell_y1 = rect.y / ETHER_ENTITY_CHUNK_CELL_SIZE;
    uint16_t cell_x2 = (rect.x + rect.w) / ETHER_ENTITY_CHUNK_CELL_SIZE;
    uint16_t cell_y2 = (rect.y + rect.h) / ETHER_ENTITY_CHUNK_CELL_SIZE;

    for (ETHER_entity_chunk_id_t j = 0; j < state->entities->chunks->len; j++)
    {
        ETHER_rect_u8 chunk_rect = state->entities->chunks->rects[j];
        ETHER_rect_u16 chunk_rect_as_world;
        chunk_rect_as_world.x = chunk_rect.x * ETHER_ENTITY_CHUNK_SIZE_WORLD;
        chunk_rect_as_world.y = chunk_rect.y * ETHER_ENTITY_CHUNK_SIZE_WORLD;
        chunk_rect_as_world.w = chunk_rect.w * ETHER_ENTITY_CHUNK_SIZE_WORLD;
        chunk_rect_as_world.h = chunk_rect.h * ETHER_ENTITY_CHUNK_SIZE_WORLD;

        if (ETHER_rects_collide_u16_2(rect, chunk_rect_as_world))
        {
            int16_t _cell_x1 = cell_x1 - chunk_rect_as_world.x / ETHER_ENTITY_CHUNK_CELL_SIZE;
            int16_t _cell_y1 = cell_y1 - chunk_rect_as_world.y / ETHER_ENTITY_CHUNK_CELL_SIZE;
            int16_t _cell_x2 = cell_x2 - chunk_rect_as_world.x / ETHER_ENTITY_CHUNK_CELL_SIZE;
            int16_t _cell_y2 = cell_y2 - chunk_rect_as_world.y / ETHER_ENTITY_CHUNK_CELL_SIZE;

            _cell_x1 = (_cell_x1 < 0) ? 0 : _cell_x1;
            _cell_y1 = (_cell_y1 < 0) ? 0 : _cell_y1;
            _cell_x2 = (_cell_x2 >= ETHER_ENTITY_CHUNK_SIZE_CELLS) ? ETHER_ENTITY_CHUNK_SIZE_CELLS - 1 : _cell_x2;
            _cell_y2 = (_cell_y2 >= ETHER_ENTITY_CHUNK_SIZE_CELLS) ? ETHER_ENTITY_CHUNK_SIZE_CELLS - 1 : _cell_y2;

            for (int16_t x = _cell_x1; x <= _cell_x2; x++)
            {
                for (int16_t y = _cell_y1; y <= _cell_y2; y++)
                {
                    uint16_t cell_i = x + y * ETHER_ENTITY_CHUNK_SIZE_CELLS;
                    ETHER_entity_chunk_cell *cell = &state->entities->chunks->chunks[j].cells[cell_i];
                    
                    array.cells[array.len] = cell;
                    array.len++;
                }
            }
        }
    }

    return array;
}

void ETHER_handle_entities_chunks(ETHER_state *state)
{
    memset(state->entities->chunks->chunks, 0, sizeof(*state->entities->chunks->chunks) * state->entities->chunks->len);

    for (ETHER_entity_id_t i = 0; i < state->entities->len; i++)
    {
        ETHER_entity_chunk_cell_array array = ETHER_get_entity_chunk_cells(state, i);
        for (uint8_t j = 0; j < array.len; j++)
        {
            ETHER_entity_chunk_cell *cell = array.cells[j];
            if (cell->len < ETHER_ENTITY_CHUNK_CELL_CAP)
            {
                cell->entities[cell->len] = i;
                cell->len++;
            }
        }
        free(array.cells);
    }
}

#define COLLISION_CONSTANT 1

void ETHER_handle_entities_collisions(ETHER_state *state)
{
    for (ETHER_entity_id_t i = 0; i < state->entities->len; i++)
    {
        int16_t force_x = 0;
        int16_t force_y = 0;

        // state->entities->rects[i].x += (rand() % 3) - 1;
        // state->entities->rects[i].y += (rand() % 3) - 1;
        state->entities->rects[i].x += SIGN2(RENDER_WIDTH / 2 - state->entities->rects[i].x);
        state->entities->rects[i].y += SIGN2(RENDER_HEIGHT / 2 - state->entities->rects[i].y);

        ETHER_entity_chunk_cell_array array = ETHER_get_entity_chunk_cells(state, i);

        for (uint8_t j = 0; j < array.len; j++)
        {
            ETHER_entity_chunk_cell *cell = array.cells[j];
            for (uint8_t k = 0; k < cell->len; k++)
            {
                ETHER_entity_id_t entity_this = i;
                ETHER_entity_id_t entity_that = cell->entities[k];
                if (entity_this == entity_that) continue;
                ETHER_rect_world_space *entity_this_rect = &state->entities->rects[entity_this];
                ETHER_rect_world_space *entity_that_rect = &state->entities->rects[entity_that];
                if (!ETHER_rects_collide_u16(*entity_this_rect, *entity_that_rect)) continue;
                int16_t delta_x = entity_that_rect->x - entity_this_rect->x;
                int16_t delta_y = entity_that_rect->y - entity_this_rect->y;
                int16_t abs_delta_x = ABS(delta_x);
                int16_t abs_delta_y = ABS(delta_y);
                // future TODO: using ETHER_ENTITY_SIZE here won't work for entities of different sizes
                if (abs_delta_x > abs_delta_y / COLLISION_CONSTANT) force_x -= CEIL((float) (ETHER_ENTITY_SIZE - abs_delta_x) / 2) * SIGN2(delta_x);
                if (abs_delta_y > abs_delta_x / COLLISION_CONSTANT) force_y -= CEIL((float) (ETHER_ENTITY_SIZE - abs_delta_y) / 2) * SIGN2(delta_y);
            }
        }

        free(array.cells);

        state->entities->rects[i].x += force_x;
        state->entities->rects[i].y += force_y;
    }
}
