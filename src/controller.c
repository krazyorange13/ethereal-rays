#include "controller.h"

#include "settings.h"

void ETHER_update(ETHER_state *state)
{
    ETHER_handle_events(state);
    ETHER_handle_mouse(state);
    ETHER_handle_entities(state);

    state->camera.y += (state->input->move_down - state->input->move_up) * 3;
    state->camera.x += (state->input->move_right - state->input->move_left) * 3;
}

void ETHER_handle_mouse(ETHER_state *state)
{
    SDL_GetMouseState(&state->mouse.x, &state->mouse.y);
}

void ETHER_handle_events(ETHER_state *state)
{
    state->smth = FALSE;
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
            state->smth = TRUE;

        SDL_Scancode code = event->key.scancode;
        HANDLE_BOUND_INPUT(move_up)
        HANDLE_BOUND_INPUT(move_down)
        HANDLE_BOUND_INPUT(move_right)
        HANDLE_BOUND_INPUT(move_left)
        HANDLE_BOUND_INPUT(smth);
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
    ETHER_handle_entities_depth_sort(state);
    ETHER_handle_entities_behavior(state);
    ETHER_handle_entities_chunks(state);
    ETHER_handle_entities_collisions(state);
    ETHER_handle_entities_behavior_2(state);
}

void ETHER_handle_entities_behavior(ETHER_state *state)
{
    if (state->input->smth)
    {
        memset(state->entities->velocities, 0, sizeof(*state->entities->velocities) * state->entities->len);
        // printf("%lld\n", state->frames);
        return;
    }
    for (ETHER_entity_id_t i = 0; i < state->entities->len; i++)
    {
        // state->entities->velocities[i].x = (rand() % 11) - 5;
        // state->entities->velocities[i].y = (rand() % 11) - 5;
        // if (rand() % 2)
        // {
        // if (!state->input->smth)
        // {
            // state->entities->velocities[i].x = SIGN((state->camera.x + state->camera.w / 2) - state->entities->rects[i].x) * 2;
            state->entities->velocities[i].y = SIGN((state->camera.y + state->camera.h / 2) - state->entities->rects[i].y) * 5;
        // }
        // }
        // else
        // {
            state->entities->velocities[i].x = SIGN((state->camera.x + state->camera.w / 2) - state->entities->rects[i].x) * 5;
            
            // state->entities->velocities[i].x = 0;
            // state->entities->velocities[i].y = 0;
            // state->entities->velocities[i].x = SIGN(state->camera.x + state->camera.w / 2 - state->entities->rects[i].x);
            // state->entities->velocities[i].y = SIGN(state->camera.y + state->camera.h / 2 - state->entities->rects[i].y);
        // }
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

void ETHER_handle_entities_collisions(ETHER_state *state)
{
    for (ETHER_entity_id_t i = 0; i < state->entities->len; i++)
    {
        ETHER_entity_chunk_cell_array array = ETHER_get_entity_chunk_cells(state, i);
        for (uint8_t j = 0; j < array.len; j++)
        {
            ETHER_entity_chunk_cell *cell = array.cells[j];
            for (uint8_t k = 0; k < cell->len; k++)
            {
                ETHER_entity_id_t l = cell->entities[k];
                if (i == l) continue;
                ETHER_handle_entities_collision(state, i, l);
            }
        }
    }
}

void ETHER_handle_entities_collision(ETHER_state *state, ETHER_entity_id_t curr, ETHER_entity_id_t othr)
{
    ETHER_rect_world_space *curr_rect = &state->entities->rects[curr];
    ETHER_rect_world_space *othr_rect = &state->entities->rects[othr];
    ETHER_rect_s16 *curr_vel = &state->entities->velocities[curr];
    ETHER_rect_s16 *othr_vel = &state->entities->velocities[othr];
    ETHER_rect_world_space curr_rect_pre = *curr_rect;
    curr_rect_pre.x += curr_vel->x;
    curr_rect_pre.y += curr_vel->y;
    ETHER_rect_world_space othr_rect_pre = *othr_rect;
    othr_rect_pre.x += othr_vel->x;
    othr_rect_pre.y += othr_vel->y;

    if (!ETHER_rects_collide_u16(curr_rect_pre, *othr_rect)) return;

    int16_t delta_x_pre = othr_rect->x - curr_rect_pre.x;
    int16_t delta_y_pre = othr_rect->y - curr_rect_pre.y;
    int16_t overlap_x_pre = ETHER_ENTITY_SIZE - ABS(delta_x_pre);
    int16_t overlap_y_pre = ETHER_ENTITY_SIZE - ABS(delta_y_pre);

    if (overlap_x_pre > 0 && overlap_x_pre <= overlap_y_pre) curr_vel->x -= overlap_x_pre * SIGN2(delta_x_pre);
    {
        // int16_t sep = FLOOR((float) overlap_x_pre * SIGN2(delta_x_pre) / 2);
        // curr_vel->x -= sep;
        // othr_vel->x += sep;
    }
    if (overlap_y_pre > 0 && overlap_y_pre <= overlap_x_pre) curr_vel->y -= overlap_y_pre * SIGN2(delta_y_pre);
    {
        // int16_t sep = FLOOR((float) overlap_y_pre * SIGN2(delta_y_pre) / 2);
        // curr_vel->y -= sep;
        // othr_vel->y += sep;
    }
    int16_t delta_x = othr_rect->x - curr_rect->x;
    int16_t delta_y = othr_rect->y - curr_rect->y;
    if (delta_x == 0 && delta_y == 0)
    {
        // state->quit = TRUE;
        // curr_vel->x = ((rand() % 3) - 1) * 1;
        // curr_vel->y = ((rand() % 3) - 1) * 1;
    }
}

void ETHER_handle_entities_behavior_2(ETHER_state *state)
{
    for (ETHER_entity_id_t i = 0; i < state->entities->len; i++)
    {
        state->entities->rects[i].x += state->entities->velocities[i].x;
        state->entities->rects[i].y += state->entities->velocities[i].y;
        if (state->entities->velocities[i].x || state->entities->velocities[i].y)
            state->entities->states[i].behavior = ETHER_ENTITY_BEHAVIOR_PERSON_WALK;
        else state->entities->states[i].behavior = ETHER_ENTITY_BEHAVIOR_PERSON_IDLE;
        if (state->entities->velocities[i].x > 0)
            state->entities->states[i].direction = ETHER_ENTITY_DIRECTION_RIGHT;
        else if (state->entities->velocities[i].x < 0)
            state->entities->states[i].direction = ETHER_ENTITY_DIRECTION_LEFT;
    }
}
