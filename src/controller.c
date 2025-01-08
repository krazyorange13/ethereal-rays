#include "controller.h"

#include "settings.h"

void ETHER_update(ETHER_state *state)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ETHER_handle_event(&event, state);
    }
    
    SDL_GetMouseState(&state->mouse.x, &state->mouse.y);

    ETHER_handle_entities(state);
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
    ETHER_entities_depth_sort(state->entities);
    for (ETHER_entity *entity = state->entities->head; entity != NULL; entity = entity->next)
    {
        ETHER_rect_u16 entity_rect_old = entity->rect;

        if (entity != state->player)
        {
            // entity->rect.x += (rand() % 5) - 2;
            // entity->rect.y += (rand() % 5) - 2;
            // entity->rect.x += SIGN2(state->mouse.x - entity->rect.x);
            // entity->rect.y += SIGN2(state->mouse.y - entity->rect.y);
            entity->rect.x += SIGN2(RENDER_WIDTH / 2 - entity->rect.x);
            entity->rect.y += SIGN2(RENDER_HEIGHT / 2 - entity->rect.y);
        }
        else
        {
            if (state->input->move_up) state->player->rect.y -= 5;
            if (state->input->move_down) state->player->rect.y += 5;
            if (state->input->move_left) state->player->rect.x -= 5;
            if (state->input->move_right) state->player->rect.x += 5;
        }

        entity->rect.x %= RENDER_WIDTH - ENTITY_SIZE;
        entity->rect.y %= RENDER_HEIGHT - ENTITY_SIZE;
        
        ETHER_handle_entity_collisions(entity);
        ETHER_handle_entity_buckets(state->quadtree, entity, entity_rect_old);
    }

}

void ETHER_handle_entity_buckets(ETHER_state_quadtree *quadtree, ETHER_entity *entity, ETHER_rect_u16 entity_rect_old)
{

    if (ETHER_rects_equal_u16(entity_rect_old, entity->rect))
        return;
    
    ETHER_rect_u8 rect_qt_old = ETHER_rect_world_to_quadtree_2(entity_rect_old);
    ETHER_rect_u8 rect_qt_new = ETHER_rect_world_to_quadtree_2(entity->rect);
    ETHER_array *leaves_old = ETHER_node_get_rect_leaves(quadtree->base, rect_qt_old);
    ETHER_array *leaves_new = ETHER_node_get_rect_leaves(quadtree->base, rect_qt_new);
    for (uint8_t i = 0; i < leaves_new->len; i++)
    {
        for (uint8_t j = 0; j < leaves_old->len; j++)
        {
            if (leaves_new->data[i] == leaves_old->data[j])
            {
                leaves_new->data[i] = NULL;
                leaves_old->data[j] = NULL;
            }
        }
    }
    for (uint8_t i = 0; i < leaves_new->len; i++)
    {
        if (leaves_new->data[i] == NULL)
            continue;
        ETHER_leaf *leaf = leaves_new->data[i];
        ETHER_bucket_add(&leaf->bucket, entity);
    }
    for (uint8_t j = 0; j < leaves_old->len; j++)
    {
        if (leaves_old->data[j] == NULL)
            continue;
        ETHER_leaf *leaf = leaves_old->data[j];
        ETHER_bucket_remove(&leaf->bucket, entity);
    }

    ETHER_array_destroy(leaves_old);
    ETHER_array_destroy(leaves_new);
}

#define COLLISION_CONSTANT 1

void ETHER_handle_entity_collisions(ETHER_entity *entity)
{
    int16_t force_x = 0;
    int16_t force_y = 0;
    for (ETHER_bucket_node *node = entity->bucket_head; node != NULL; node = node->next)
    {
        ETHER_bucket *bucket = node->curr;
        for (ETHER_entity_node *other = bucket->entity_head; other != NULL; other = other->next)
        {
            if (entity == other->curr) continue;
            ETHER_rect_u16 *entity_rect = &entity->rect;
            ETHER_rect_u16 *other_rect = &other->curr->rect;
            if (!ETHER_rects_collide_u16_2(*entity_rect, *other_rect)) continue;
            int16_t delta_x = other_rect->x - entity_rect->x;
            int16_t delta_y = other_rect->y - entity_rect->y;
            uint16_t abs_delta_x = ABS(delta_x);
            uint16_t abs_delta_y = ABS(delta_y);
            if (abs_delta_x > abs_delta_y / COLLISION_CONSTANT) force_x -= CEIL((float) (ENTITY_SIZE - abs_delta_x) / 2) * SIGN(delta_x);
            if (abs_delta_y > abs_delta_x / COLLISION_CONSTANT) force_y -= CEIL((float) (ENTITY_SIZE - abs_delta_y) / 2) * SIGN(delta_y);
        }
    }
    entity->rect.x += force_x;
    entity->rect.y += force_y;
}

void ETHER_entities_debug(ETHER_state_entities *entities, ETHER_entity *marker0, ETHER_entity *marker1, ETHER_entity *marker2)
{
    printf("\t");
    for (ETHER_entity *entity = entities->head; entity != NULL; entity = entity->next)
    {
        if (entity == marker0) printf("\x1b[31m");
        if (entity == marker1) printf("\x1b[32m");
        if (entity == marker2) printf("\x1b[33m");
        printf("%d\x1b[0m ", entity->rect.y);
    }
    printf("\n");
}

#define SORT_CHECK(left, right) (!left || (right->rect.y <= left->rect.y))

void ETHER_entities_depth_sort(ETHER_state_entities *entities)
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
