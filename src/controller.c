#include "controller.h"

#include "settings.h"

void handle_event(SDL_Event *event, ETHER_state *state);
void ETHER_insertion_sort(ETHER_state_entities *entities);

void ETHER_entities_debug(ETHER_state_entities *entities, ETHER_entity *marker0, ETHER_entity *marker1, ETHER_entity *marker2)
{
    printf("\t");
    for (ETHER_entity *entity = entities->head; entity != NULL; entity = entity->next)
    {
        if (entity == marker0) printf("\x1b[31m");
        if (entity == marker1) printf("\x1b[32m");
        if (entity == marker2) printf("\x1b[33m");
        printf("%d\x1b[0m ", entity->pos.y);
    }
    printf("\n");
}

void ETHER_update(ETHER_state *state)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        handle_event(&event, state);
    }
    
    SDL_GetMouseState(&state->mouse.x, &state->mouse.y);

    for (ETHER_entity *p = state->entities->head; p != NULL; p = p->next)
    {
        p->pos.x += (rand() % 3 - 1);
        p->pos.y += (rand() % 3 - 1);
    }
    
    // sort entities by depth
    ETHER_insertion_sort(state->entities);
}

#define HANDLE_BOUND_INPUT(bind) \
    if (code == state->keybinds->bind) \
        state->input->bind = (event->type == SDL_EVENT_KEY_DOWN); \

void handle_event(SDL_Event *event, ETHER_state *state)
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

#define SORT_CHECK(left, right) (!left || (right->pos.y >= left->pos.y))

void ETHER_insertion_sort(ETHER_state_entities *entities)
{
    if (!entities->head || !entities->head->next)
        return;

    ETHER_entity *head = entities->head->next;
    ETHER_entity *prev = entities->head;

    while (head)
    {
        if (SORT_CHECK(prev, head))
        {
            prev = prev->next;
            head = prev->next;
        }
        else
        {
            ETHER_entity *temp = prev;
            while (1)
            {
                if (SORT_CHECK(temp, head))
                {
                    if (entities->tail == head)
                        entities->tail = head->prev;
                    ETHER_move_entity(head, temp);
                    head = prev->next;
                    break;
                }
                else
                {
                    temp = temp->prev;
                    if (temp == NULL)
                    {
                        ETHER_entity *dest_next = entities->head;
                        ETHER_entity *curr_prev = head->prev;
                        ETHER_entity *curr_next = head->next;
                        head->prev = NULL;
                        head->next = dest_next;
                        dest_next->prev = head;
                        entities->head = head;
                        if (curr_prev) curr_prev->next = curr_next;
                        if (curr_prev && entities->tail == head)
                            entities->tail = curr_prev;
                        if (curr_next) curr_next->prev = curr_prev;
                        head = prev->next;
                        break;
                    }
                }
            }
        }
    }
}
