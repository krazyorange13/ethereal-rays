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

        entity->rect.x += (rand() % 3) - 1;
        entity->rect.y += (rand() % 3) - 1;
        entity->rect.x %= RENDER_WIDTH;
        entity->rect.y %= RENDER_HEIGHT;
        
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

void ETHER_handle_entity_collisions(ETHER_entity *entity)
{
    for (ETHER_bucket_node *node = entity->bucket_head; node != NULL; node = node->next)
    {
        ETHER_bucket *bucket = node->curr;
        for (ETHER_entity_node *other = bucket->entity_head; other != NULL; other = other->next)
        {
            if (entity == other->curr) continue;
            ETHER_rect_u16 *entity_rect = &entity->rect;
            ETHER_rect_u16 *other_rect = &other->curr->rect;
            if (!ETHER_rect_overlap_rect_u16(*entity_rect, *other_rect)) continue;
            int16_t delta_x = other_rect->x - entity_rect->x;
            int16_t delta_y = other_rect->y - entity_rect->y;
            int16_t move_x = ((ABS(delta_x))) * 0.125 * SIGN(delta_x);
            int16_t move_y = ((ABS(delta_y))) * 0.125 * SIGN(delta_y);
            entity_rect->x -= move_x;
            entity_rect->y -= move_y;
            break;
        }
    }
}

void ETHER_handle_entities_collisions(ETHER_state_quadtree *quadtree)
{
    for (ETHER_leaf *leaf = quadtree->leaves_head; leaf != NULL; leaf = leaf->next)
    {
        for (ETHER_entity_node *entity_node = leaf->bucket.entity_head; entity_node != NULL; entity_node = entity_node->next)
        {
            for (ETHER_entity_node *other_node = leaf->bucket.entity_head; other_node != NULL; other_node = other_node->next)
            {
                if (entity_node == other_node) continue;
                ETHER_rect_u16 *entity_rect = &entity_node->curr->rect;
                ETHER_rect_u16 *other_rect = &other_node->curr->rect;
                if (!ETHER_rect_overlap_rect_u16(*entity_rect, *other_rect)) continue;
                int16_t delta_x = other_rect->x - entity_rect->x;
                int16_t delta_y = other_rect->y - entity_rect->y;
                int16_t move_x = ((ENTITY_SIZE / 2) - ABS(delta_x)) * SIGN(delta_x);
                int16_t move_y = ((ENTITY_SIZE / 2) - ABS(delta_y)) * SIGN(delta_y);
                entity_rect->x -= move_x;
                entity_rect->y -= move_y;
                other_rect->x += move_x;
                other_rect->y += move_y;
            }
        }
    }
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
