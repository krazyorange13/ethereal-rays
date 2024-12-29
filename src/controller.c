#include "controller.h"

#include "settings.h"

void handle_event(SDL_Event *event, ETHER_state *state);
void ETHER_insertion_sort(ETHER_state_entities *entities);

void ETHER_move_entity(ETHER_entity *curr, ETHER_entity *dest_prev);
void ETHER_swap_entities(ETHER_entity *prev, ETHER_entity *next);
void ETHER_swap_entities_adjacent(ETHER_entity *prev, ETHER_entity *next);
void ETHER_swap_entities_nonadjacent(ETHER_entity *prev, ETHER_entity *next);

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
    // printf("\n");
    // getchar();
}

void ETHER_update(ETHER_state *state)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        handle_event(&event, state);
    }

    for (ETHER_entity *p = state->entities->head; p != NULL; p = p->next)
    {
        p->pos.x += (rand() % 3 - 1) % RENDER_WIDTH;
        p->pos.y += (rand() % 3 - 1) % RENDER_HEIGHT;
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

        SDL_Scancode code = event->key.scancode;
        HANDLE_BOUND_INPUT(move_up)
        HANDLE_BOUND_INPUT(move_down)
        HANDLE_BOUND_INPUT(move_right)
        HANDLE_BOUND_INPUT(move_left)
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

/*
 * Entity list operations
 */

void ETHER_move_entity(ETHER_entity *curr, ETHER_entity *dest_prev)
{
    ETHER_entity *dest_next = dest_prev->next;
    ETHER_entity *curr_prev = curr->prev;
    ETHER_entity *curr_next = curr->next;

    dest_prev->next = curr;
    curr->prev = dest_prev;
    curr->next = dest_next;
    dest_next->prev = curr;
    curr_prev->next = curr_next;
    if (curr_next) curr_next->prev = curr_prev;
}

void ETHER_swap_entities(ETHER_entity *prev, ETHER_entity *next)
{
    if (prev->next == next && next->prev == prev)
        ETHER_swap_entities_adjacent(prev, next);
    else
        ETHER_swap_entities_nonadjacent(prev, next);
}

void ETHER_swap_entities_adjacent(ETHER_entity *prev, ETHER_entity *next)
{
    ETHER_entity *prev_prev = prev->prev;
    ETHER_entity *next_next = next->next;

    prev_prev->next = next;
    next->prev = prev_prev;
    next->next = prev;
    prev->prev = next;
    prev->next = next_next;
    next_next->prev = prev;
}

void ETHER_swap_entities_nonadjacent(ETHER_entity *prev, ETHER_entity *next)
{
    ETHER_entity *prev_prev = prev->prev;
    ETHER_entity *prev_next = prev->next;
    ETHER_entity *next_prev = next->prev;
    ETHER_entity *next_next = next->next;

    prev_prev->next = next;
    next->prev = prev_prev;
    next->next = prev_next;
    prev_next->prev = next;
    next_prev->next = prev;
    prev->prev = next_prev;
    prev->next = next_next;
    next_next->prev = prev;
}


void ETHER_entities_add(ETHER_state_entities *entities, ETHER_entity *entity)
{
    if (entities->count)
    {
        entities->tail->next = entity;
        entity->prev = entities->tail;
        entities->tail = entity;
        entity->next = NULL;
    }
    else
    {
        entities->head = entity;
        entities->tail = entity;
        entity->next = NULL;
        entity->prev = NULL;
    }
    entities->count++;
}

ETHER_entity *ETHER_entities_pop(ETHER_state_entities *entities)
{
    if (entities->count)
    {
        ETHER_entity *p = entities->tail;
        p->prev->next = NULL;
        p->prev = NULL;
        return p;
    }
    else
    {
        return NULL;
    }
}

/*
 * Quadtree structure operations
 */

void _ETHER_node_debug(ETHER_node *node, uint8_t depth);

void ETHER_node_debug(ETHER_node *node)
{
    _ETHER_node_debug(node, 0);
}

void _ETHER_node_debug(ETHER_node *node, uint8_t depth)
{
    if (node == NULL)
        return;

    printf("%*snode\n", depth * 2, "");
    
    for (uint8_t i = 0; i < 4; i++)
    {
        _ETHER_node_debug(node->quad[i], depth + 1);
    }
}

void ETHER_node_create(ETHER_node **node, ETHER_node *parent)
{
    *node = malloc(sizeof(ETHER_node));
    (*node)->quad = malloc(sizeof(ETHER_node *) * 4);
    memset((*node)->quad, 0, sizeof(ETHER_node *) * 4);
    (*node)->leaf = NULL;
    (*node)->parent = parent;
    (*node)->rect = ETHER_node_get_rect(*node);
}

void ETHER_node_subdivide(ETHER_node *node)
{
    if (node->quad[0] != NULL || node->quad[1] != NULL
     || node->quad[2] != NULL || node->quad[3] != NULL)
    {
        printf("oopsies bad subdivide\n");
        return;
    }
    ETHER_node **nodes = malloc(sizeof(ETHER_node *) * 4);
    ETHER_node_create(&nodes[0], node);
    ETHER_node_create(&nodes[1], node);
    ETHER_node_create(&nodes[2], node);
    ETHER_node_create(&nodes[3], node);
    node->quad[0] = nodes[0];
    node->quad[1] = nodes[1];
    node->quad[2] = nodes[2];
    node->quad[3] = nodes[3];
}

uint8_t ETHER_node_get_parent_pos(ETHER_node *node)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        if (node->parent->quad[i] == node)
            return i;
    }
    return 4;
}

ETHER_rect ETHER_node_get_rect(ETHER_node *node)
{
    if (node->parent == NULL)
    {
        ETHER_rect rect = {{0, 0}, {ETHER_STATE_QUADTREE_MAX_SIZE, ETHER_STATE_QUADTREE_MAX_SIZE}};
        return rect;
    }

    ETHER_rect prect = node->parent->rect; //ETHER_node_get_rect(node->parent);
    uint8_t pos = ETHER_node_get_parent_pos(node);

    if (pos == 4)
    {
        printf("BAD BAD BAD BAD\n");
    }

    ETHER_rect rect;
    rect.pos.x = prect.pos.x + ((prect.dim.x * (pos % 2)) >> 1);
    rect.pos.y = prect.pos.y + ((prect.dim.y * (pos / 2)) >> 1);
    rect.dim.x = prect.dim.x >> 1;
    rect.dim.y = prect.dim.y >> 1;

    return rect;
}