#ifndef _ETHER_STATE_H
#define _ETHER_STATE_H

#include "utils.h"

#include "SDL3/SDL.h"

typedef struct _ETHER_state ETHER_state;
typedef struct _ETHER_state_input ETHER_state_input;
typedef struct _ETHER_state_keybinds ETHER_state_keybinds;
typedef struct _ETHER_state_entities ETHER_state_entities;
typedef struct _ETHER_state_textures ETHER_state_textures;
typedef struct _ETHER_state_quadtree ETHER_state_quadtree;

typedef struct _ETHER_entity ETHER_entity;

ETHER_state_keybinds create_default_state_keybinds();

struct _ETHER_state
{
    double fps;
    BOOL quit;
    ETHER_state_input *input;
    ETHER_state_keybinds *keybinds;
    ETHER_state_textures *textures;
    ETHER_state_entities *entities;
    ETHER_state_quadtree *quadtree;
};

struct _ETHER_state_input
{
    bool move_up;
    bool move_down;
    bool move_left;
    bool move_right;
};

struct _ETHER_state_keybinds
{
    SDL_Scancode move_up;
    SDL_Scancode move_down;
    SDL_Scancode move_right;
    SDL_Scancode move_left;
};

struct _ETHER_state_entities
{
    uint16_t count;
    ETHER_entity *head;
    ETHER_entity *tail;
};

struct _ETHER_entity
{
    ETHER_vec2_u16 pos;
    SDL_Texture *texture;
    struct _ETHER_entity *next;
    struct _ETHER_entity *prev;
};

void ETHER_move_entity(ETHER_entity *curr, ETHER_entity *dest_prev);
void ETHER_swap_entities(ETHER_entity *prev, ETHER_entity *next);
void ETHER_swap_entities_adjacent(ETHER_entity *prev, ETHER_entity *next);
void ETHER_swap_entities_nonadjacent(ETHER_entity *prev, ETHER_entity *next);
void ETHER_entities_add(ETHER_state_entities *entities, ETHER_entity *entity);
ETHER_entity *ETHER_entities_pop(ETHER_state_entities *entities);

struct _ETHER_state_textures
{
    SDL_Texture *player;
    SDL_Texture *gem;
    SDL_Texture *leaf;
};

void ETHER_state_textures_load(SDL_Renderer *renderer, ETHER_state_textures *textures);

#define BLOCK_SIZE 16
#define CHUNK_SIZE 16
#define QUADTREE_SIZE 255
#define CHUNK_WORLD_SIZE BLOCK_SIZE * CHUNK_SIZE

typedef uint8_t block_id_t;
typedef uint8_t chunk_coord_t;
typedef uint8_t quadtree_coord_t;
typedef uint16_t world_coord_t;
typedef struct _ETHER_chunk ETHER_chunk;
typedef union _ETHER_branch ETHER_branch;
typedef struct _ETHER_node ETHER_node;
typedef struct _ETHER_leaf ETHER_leaf;

struct _ETHER_state_quadtree
{
    ETHER_node *base;
};

struct _ETHER_chunk
{
    block_id_t blocks[CHUNK_SIZE * CHUNK_SIZE];
};

union _ETHER_branch
{
    ETHER_node *node;
    ETHER_leaf *leaf;
};

struct _ETHER_node
{
    ETHER_node *parent;
    uint8_t ppos;
    uint8_t depth;
    ETHER_rect_u8 rect;
    ETHER_branch *quad;
};

struct _ETHER_leaf
{
    ETHER_chunk chunk;
};

void ETHER_node_create(ETHER_node **node, ETHER_node *parent, uint8_t ppos);
void ETHER_node_subdivide(ETHER_node *node);
uint8_t ETHER_node_get_depth(ETHER_node *node);
uint8_t ETHER_node_get_ppos(ETHER_node *node);
ETHER_rect_u8 ETHER_node_get_rect(ETHER_node *node);
void ETHER_node_debug(ETHER_node *node);

#endif