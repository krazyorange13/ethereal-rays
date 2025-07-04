#include "controller.h"

#include "settings.h"

void ETHER_update(ETHER_state *state)
{
    ETHER_handle_events(state);
    ETHER_handle_mouse(state);
    ETHER_handle_entities(state);

    state->camera.x += (state->input->move_right - state->input->move_left) * 8 / RENDER_RATIO;
    state->camera.y += (state->input->move_down - state->input->move_up) * 8 / RENDER_RATIO;
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

// TODO: optimize this
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
        ETHER_rect_s16 velocity = entities->velocities[i];
        ETHER_entity_state state = entities->states[i];
        ETHER_entity_type_t type = entities->types[i];

        ETHER_entity_id_t j = i;

        while(j != 0 && rect.y < entities->rects[j - 1].y)
        {
            entities->rects[j] = entities->rects[j - 1];
            entities->velocities[j] = entities->velocities[j - 1];
            entities->states[j] = entities->states[j - 1];
            entities->types[j] = entities->types[j - 1];

            j--;
        }

        entities->rects[j] = rect;
        entities->velocities[j] = velocity;
        entities->states[j] = state;
        entities->types[j] = type;
    }
}

void ETHER_handle_entities_behavior(ETHER_state *state)
{
    for (ETHER_entity_id_t i = 0; i < state->entities->len; i++)
    {
        ETHER_handle_entity_behavior(state, i);
    }
}

void ETHER_handle_entity_behavior(ETHER_state *state, ETHER_entity_id_t entity)
{
    if (state->entities->types[entity] == ETHER_ENTITY_TYPE_PLAYER)
    {
        state->entities->velocities[entity].x = (state->input->move_right - state->input->move_left) * 4;
        state->entities->velocities[entity].y = (state->input->move_down - state->input->move_up) * 4;
        return;
    }
    
    int target_x = state->camera.x + state->mouse.x / RENDER_RATIO;
    int target_y = state->camera.y + state->mouse.y / RENDER_RATIO;
    // if (ABS(target_x - state->entities->rects[entity].x) > 100)
        state->entities->velocities[entity].x = SIGN2(target_x - state->entities->rects[entity].x) * 4;// * (rand() % 2);
    // if (ABS(target_y - state->entities->rects[entity].y) > 100)
        state->entities->velocities[entity].y = SIGN2(target_y - state->entities->rects[entity].y) * 4;// * (rand() % 2);

}

typedef struct
{
    uint8_t len;
    uint8_t cap;
    ETHER_entity_chunk_cell **cells;
} ETHER_entity_chunk_cell_array;

// TODO: optimize this!!!
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
        ETHER_handle_entity_chunks(state, i);
    }
}

// TODO: optimize this!!
void ETHER_handle_entity_chunks(ETHER_state *state, ETHER_entity_id_t entity)
{
    ETHER_entity_chunk_cell_array array = ETHER_get_entity_chunk_cells(state, entity);
    for (uint8_t i = 0; i < array.len; i++)
    {
        ETHER_entity_chunk_cell *cell = array.cells[i];
        if (cell->len < ETHER_ENTITY_CHUNK_CELL_CAP)
        {
            cell->entities[cell->len] = entity;
            cell->len++;
        }
    }
    free(array.cells);
}

void ETHER_handle_entities_collisions(ETHER_state *state)
{
    for (ETHER_entity_id_t i = 0; i < state->entities->len; i++)
    {
        ETHER_handle_entity_collisions(state, i);
    }
}

// TODO: optimize this!!!
void ETHER_handle_entity_collisions(ETHER_state *state, ETHER_entity_id_t entity)
{
    // if (state->entities->types[entity] == ETHER_ENTITY_TYPE_PLAYER)
    // {
    //     state->entities->rects[entity].x += state->entities->velocities[entity].x;
    //     state->entities->rects[entity].y += state->entities->velocities[entity].y;
    //     return;
    // }

    ETHER_rect_world_space curr_rect = state->entities->rects[entity];
    ETHER_rect_world_space desired_rect = curr_rect;
    desired_rect.x += state->entities->velocities[entity].x;
    desired_rect.y += state->entities->velocities[entity].y;
    ETHER_rect_world_space cumulative_rect = desired_rect;

#define PLAYER_PRIORITY 16

    if (state->entities->types[entity] == ETHER_ENTITY_TYPE_PLAYER)
    {
        cumulative_rect.x += desired_rect.x * PLAYER_PRIORITY;
        cumulative_rect.y += desired_rect.y * PLAYER_PRIORITY;
    }

#define COLL_ENTS_LEN_MAX 64
    ETHER_entity_id_t coll_ents[COLL_ENTS_LEN_MAX] = {0};
    ETHER_entity_id_t coll_ents_len = 0;

    // this is a second invocation of ETHER_get_entity_chunk_cells,
    //      optimize by caching result
    // only keep unique entities
    ETHER_entity_chunk_cell_array array = ETHER_get_entity_chunk_cells(state, entity);
    for (int i = 0; i < array.len; i++)
    {
        ETHER_entity_chunk_cell *cell = array.cells[i];
        for (int j = 0; j < cell->len; j++)
        {
            ETHER_entity_id_t othr = cell->entities[j];

            // are we COLLIDING?

            if (!ETHER_rects_collide_u16(curr_rect, state->entities->rects[othr]))
                continue;

            // is this entity US?
            if (entity == othr) continue;

            // is this entity A DUPLICATE? (already in the list)
            int dup = 0;
            for (int k = 0; k < coll_ents_len; k++)
            {
                if (coll_ents[k] == othr)
                {
                    dup = 1;
                    break;
                }
            }
            if (dup) continue;

            // since we've survived, add to list
            if (coll_ents_len < COLL_ENTS_LEN_MAX)
            {
                coll_ents[coll_ents_len] = othr;
                coll_ents_len++;
            }
        }
    }

    for (int i = 0; i < coll_ents_len; i++)
    {
        ETHER_entity_id_t othr = coll_ents[i];

        ETHER_rect_world_space othr_rect = state->entities->rects[othr];
        if (!ETHER_rects_collide_u16(curr_rect, othr_rect)) continue;

        ETHER_rect_world_space corrected_rect = curr_rect;

        uint16_t abs_delta_x = ABS(curr_rect.x - othr_rect.x);
        uint16_t abs_delta_y = ABS(curr_rect.y - othr_rect.y);

        #define OFFSET (0)
        // int OFFSET = coll_ents_len / 2;

        if (abs_delta_x >= abs_delta_y)
        {
            if (curr_rect.x < othr_rect.x)
                corrected_rect.x = othr_rect.x - curr_rect.w - OFFSET;
            else
                corrected_rect.x = othr_rect.x + othr_rect.w + OFFSET;
        }
        if (abs_delta_y >= abs_delta_x)
        {
            if (curr_rect.y < othr_rect.y)
                corrected_rect.y = othr_rect.y - curr_rect.h - OFFSET;
            else
                corrected_rect.y = othr_rect.y + othr_rect.h + OFFSET;
        }

        if (abs_delta_x == 0 && abs_delta_y == 0)
        {
            if (entity > othr)
            {
                corrected_rect.x += ETHER_ENTITY_SIZE / 2;
                corrected_rect.y += ETHER_ENTITY_SIZE / 2;
            }
            else
            {
                corrected_rect.x -= ETHER_ENTITY_SIZE / 2;
                corrected_rect.y -= ETHER_ENTITY_SIZE / 2;
            }
        }

        // printf("[%d]\t\t\t%d,%d;%d,%d\n", entity, cumulative_rect.x, cumulative_rect.y, corrected_rect.x, corrected_rect.y);
        cumulative_rect.x += corrected_rect.x;
        cumulative_rect.y += corrected_rect.y;
    }

    /* crowding compensation */

    if (coll_ents_len > 6)
    {
        cumulative_rect.x -= state->entities->velocities[entity].x * 2;
        cumulative_rect.y -= state->entities->velocities[entity].y * 2;
    }

    /* */

#define DIVIDE_ROUND_DOWN(X, Y) ((X) / (Y))
#define DIVIDE_ROUND_UP(X, Y) (((X) + (Y) - 1) / (Y))

    ETHER_rect_world_space average_rect = cumulative_rect;
    // printf("[%d]\t\t%d,%d;%d,%d;\n", entity, average_rect.x, average_rect.y, curr_rect.x, curr_rect.y);

    int priority = (state->entities->types[entity] == ETHER_ENTITY_TYPE_PLAYER) * PLAYER_PRIORITY;

    uint16_t x_div_up = DIVIDE_ROUND_UP(cumulative_rect.x, coll_ents_len + 1 + priority);
    uint16_t x_div_down = DIVIDE_ROUND_DOWN(cumulative_rect.x, coll_ents_len + 1 + priority);
    uint16_t y_div_up = DIVIDE_ROUND_UP(cumulative_rect.y, coll_ents_len + 1 + priority);
    uint16_t y_div_down = DIVIDE_ROUND_DOWN(cumulative_rect.y, coll_ents_len + 1 + priority);
    if (x_div_up != curr_rect.x) average_rect.x = x_div_up;
    else average_rect.x = x_div_down;
    if (y_div_up != curr_rect.y) average_rect.y = y_div_up;
    else average_rect.y = y_div_down;

    // TODO: this player code is just testing
    //       we'll want to implement a mass value that works similar to what
    //       we're doing with multiple self values for the average calculation

    // TODO: improve cache by bundling position and velocity
    //       also saves half the space if we separate dimensions
    //       and more cache stuff

    // int16_t delta_x = average_rect.x - curr_rect.x;
    // int16_t delta_y = average_rect.y - curr_rect.y;

    // state->entities->velocities[entity].x = delta_x;
    // state->entities->velocities[entity].y = delta_y;
    // state->entities->velocities[entity].x = 0;
    // state->entities->velocities[entity].y = 0;
    state->entities->rects[entity] = average_rect;

    // ETHER_vec2_u16 mouse = {state->mouse.x / RENDER_RATIO + state->camera.x, state->mouse.y / RENDER_RATIO + state->camera.y};
    // if (ETHER_vec2_in_rect_u16(mouse, curr_rect))
    // if (delta_x == 0 && delta_y == 0)
    // if (coll_ents_len == 0)
        // printf("[%d]\t{%d,%d};[%d,%d];%d\n", entity, curr_rect.x, curr_rect.y, average_rect.x, average_rect.y, coll_ents_len);
}

void ETHER_handle_entities_behavior_2(ETHER_state *state)
{
    for (ETHER_entity_id_t i = 0; i < state->entities->len; i++)
    {
        ETHER_handle_entity_behavior_2(state, i);
    }
}

void ETHER_handle_entity_behavior_2(ETHER_state *state, ETHER_entity_id_t entity)
{
    if (state->entities->types[entity] == ETHER_ENTITY_TYPE_PLAYER)
    {
        state->camera.x = state->entities->rects[entity].x - state->camera.w / 2;
        state->camera.y = state->entities->rects[entity].y - state->camera.h / 2;
    }
    // state->entities->rects[entity].x += state->entities->velocities[entity].x;
    // state->entities->rects[entity].y += state->entities->velocities[entity].y;
    if (state->entities->velocities[entity].x || state->entities->velocities[entity].y)
        state->entities->states[entity].behavior = ETHER_ENTITY_BEHAVIOR_PERSON_WALK;
    else state->entities->states[entity].behavior = ETHER_ENTITY_BEHAVIOR_PERSON_IDLE;
    if (state->entities->velocities[entity].x > 0)
        state->entities->states[entity].direction = ETHER_ENTITY_DIRECTION_RIGHT;
    else if (state->entities->velocities[entity].x < 0)
        state->entities->states[entity].direction = ETHER_ENTITY_DIRECTION_LEFT;

    state->entities->velocities[entity].x = 0;
    state->entities->velocities[entity].y = 0;
}
