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

void ETHER_leaf_create(ETHER_leaf **leaf, ETHER_node *parent)
{
    if (!parent) return;
    (*leaf) = malloc(sizeof(ETHER_leaf));
    (*leaf)->parent = parent;
    (*leaf)->rect_quadtree = parent->rect;
    (*leaf)->rect_world = ETHER_rect_quadtree_to_world(parent->rect);
    memset((*leaf)->chunk.blocks, 0, sizeof((*leaf)->chunk.blocks));
    parent->branch.leaf = (*leaf);
    parent->is_leaf = TRUE;
}

void ETHER_node_create(ETHER_node **node, ETHER_node *parent, uint8_t ppos)
{
    (*node) = malloc(sizeof(ETHER_node));
    (*node)->parent = parent;
    (*node)->ppos = ppos;
    (*node)->depth = (parent ? parent->depth + 1 : 0);//ETHER_node_get_depth(*node);
    (*node)->rect = ETHER_node_get_rect(*node);
    (*node)->branch.quad = malloc(sizeof(ETHER_node *) * 4);
    memset((*node)->branch.quad, 0, sizeof(ETHER_node *) * 4);
    (*node)->is_leaf = FALSE;
    if (parent) (*node)->parent->branch.quad[ppos] = (*node);
}

void ETHER_node_subdivide(ETHER_node *node)
{
    if (!ETHER_node_isend(node))
        return;

    ETHER_node *quad0;
    ETHER_node *quad1;
    ETHER_node *quad2;
    ETHER_node *quad3;
    ETHER_node_create(&quad0, node, 0);
    ETHER_node_create(&quad1, node, 1);
    ETHER_node_create(&quad2, node, 2);
    ETHER_node_create(&quad3, node, 3);
}

ETHER_leaf *ETHER_node_create_leaf(ETHER_node *node, ETHER_vec2_u8 pos)
{
    if (!ETHER_vec2_in_rect_u8(pos, node->rect))
        return NULL;

    ETHER_node *curr = node;
    while (curr->depth < QUADTREE_DEPTH)
    {
        ETHER_node_subdivide(curr);
        uint8_t q = ETHER_node_get_quadrant(curr, pos);
        curr = curr->branch.quad[q];
    }
    
    ETHER_leaf *leaf;
    ETHER_leaf_create(&leaf, curr);
    return leaf;
}

BOOL ETHER_node_isend(ETHER_node *node)
{
    return node != NULL && !node->is_leaf
        && node->branch.quad[0] == NULL
        && node->branch.quad[1] == NULL
        && node->branch.quad[2] == NULL
        && node->branch.quad[3] == NULL;
}

uint8_t ETHER_node_get_quadrant(ETHER_node *node, ETHER_vec2_u8 pos)
{
    uint8_t quad = 0;
    if (pos.x > node->rect.x + node->rect.w / 2)
        quad += 1;
    if (pos.y > node->rect.y + node->rect.h / 2)
        quad += 2;
    return quad;
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
        if (node->parent->branch.quad[i] == node)
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

ETHER_rect_u16 ETHER_rect_quadtree_to_world(ETHER_rect_u8 rect)
{
    return (ETHER_rect_u16) {
        rect.x * CHUNK_WORLD_SIZE,
        rect.y * CHUNK_WORLD_SIZE,
        rect.w * CHUNK_WORLD_SIZE,
        rect.h * CHUNK_WORLD_SIZE
    };
}

void _ETHER_node_debug(ETHER_node *node, uint8_t depth)
{
    if (node == NULL)
        return;

    printf("%*s%d node %d ", node->depth * 2, "", node->depth, node->ppos);
    ETHER_rect_u8_debug_inline(node->rect);
    printf("\n");

    if (node->is_leaf)
        return;
    
    for (uint8_t i = 0; i < 4; i++)
    {
        _ETHER_node_debug(node->branch.quad[i], depth + 1);
    }
}

void ETHER_node_debug(ETHER_node *node)
{
    _ETHER_node_debug(node, 0);
}
