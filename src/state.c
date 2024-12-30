#include "state.h"
#include "settings.h"

#include "SDL3_image/SDL_image.h"

ETHER_state_keybinds create_default_state_keybinds()
{
    ETHER_state_keybinds state;
    state.move_up = STATE_DEFAULT_KEYBINDS_MOVE_UP;
    state.move_down = STATE_DEFAULT_KEYBINDS_MOVE_DOWN;
    state.move_left = STATE_DEFAULT_KEYBINDS_MOVE_LEFT;
    state.move_right = STATE_DEFAULT_KEYBINDS_MOVE_RIGHT;
    return state;
}

void ETHER_state_textures_load(SDL_Renderer *renderer, ETHER_state_textures *textures)
{
    textures->player = IMG_LoadTexture(renderer, "res/player.png");
    SDL_SetTextureScaleMode(textures->player, SDL_SCALEMODE_NEAREST);

    textures->gem = IMG_LoadTexture(renderer, "res/gem.png");
    SDL_SetTextureScaleMode(textures->gem, SDL_SCALEMODE_NEAREST);

    textures->leaf = IMG_LoadTexture(renderer, "res/leaf.png");
    SDL_SetTextureScaleMode(textures->leaf, SDL_SCALEMODE_NEAREST);
}

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

void ETHER_node_create(ETHER_node **node, ETHER_node *parent, uint8_t ppos)
{
    (*node) = malloc(sizeof(ETHER_node));
    (*node)->parent = parent;
    (*node)->ppos = ppos;
    (*node)->depth = ETHER_node_get_depth(*node);
    (*node)->rect = ETHER_node_get_rect(*node);
    (*node)->quad = malloc(sizeof(ETHER_branch) * 4);
    memset((*node)->quad, 0, sizeof(ETHER_branch) * 4);
    if (parent) (*node)->parent->quad[ppos].node = (*node);
}

void ETHER_node_subdivide(ETHER_node *node)
{
    ETHER_node *quad0;
    ETHER_node *quad1;
    ETHER_node *quad2;
    ETHER_node *quad3;
    ETHER_node_create(&quad0, node, 0);
    ETHER_node_create(&quad1, node, 1);
    ETHER_node_create(&quad2, node, 2);
    ETHER_node_create(&quad3, node, 3);
}

uint8_t ETHER_node_get_depth(ETHER_node *node)
{
    uint8_t d = 0;
    ETHER_node *p = node;
    while (p->parent)
    {
        d++;
        p = p->parent;
    }
    return d;
}

uint8_t ETHER_node_get_ppos(ETHER_node *node)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        if (node->parent->quad[i].node == node)
            return i;
    }
    return 4;
}

ETHER_rect_u8 ETHER_node_get_rect(ETHER_node *node)
{
    if (node->parent == NULL)
        return (ETHER_rect_u8) {0, 0, QUADTREE_SIZE, QUADTREE_SIZE};
    
    ETHER_rect_u8 prec = node->parent->rect;
    uint8_t ppos = node->ppos;

    switch (ppos)
    {
    case 0:
        return (ETHER_rect_u8) { prec.x, prec.y, prec.w / 2, prec.h / 2 };
    case 1:
        return (ETHER_rect_u8) {
            prec.x + 1 + prec.w / 2,
            prec.y,
            prec.w / 2,
            prec.h / 2
        };
    case 2:
        return (ETHER_rect_u8) {
            prec.x,
            prec.y + 1 + prec.h / 2,
            prec.w / 2,
            prec.h / 2
        };
    case 3:
        return (ETHER_rect_u8) {
            prec.x + 1 + prec.w / 2,
            prec.y + 1 + prec.h / 2,
            prec.w / 2,
            prec.h / 2
        };
    default:
        return (ETHER_rect_u8) { 0, 0, 0, 0 };
    }
}

void _ETHER_node_debug(ETHER_node *node, uint8_t depth)
{
    if (node == NULL)
        return;

    printf("%*s%d node %d ", depth * 2, "", node->depth, node->ppos);
    ETHER_rect_u8_debug_inline(node->rect);
    printf("\n");
    
    for (uint8_t i = 0; i < 4; i++)
    {
        _ETHER_node_debug(node->quad[i].node, depth + 1);
    }
}

void ETHER_node_debug(ETHER_node *node)
{
    _ETHER_node_debug(node, 0);
}
