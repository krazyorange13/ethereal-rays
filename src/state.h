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
    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    double fps;
    BOOL quit;
    BOOL update_textures;
    ETHER_vec2_float mouse;
    ETHER_state_input *input;
    ETHER_state_keybinds *keybinds;
    ETHER_state_textures *textures;
    ETHER_state_entities *entities;
    ETHER_state_quadtree *quadtree;
};

struct _ETHER_state_input
{
    BOOL move_up;
    BOOL move_down;
    BOOL move_left;
    BOOL move_right;
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
    ETHER_rect_u16 rect;
    SDL_Texture *tex;
    struct _ETHER_entity *next;
    struct _ETHER_entity *prev;
};

void ETHER_entity_create(ETHER_entity **entity);
void ETHER_entity_create_prealloc(ETHER_entity **entity);
void ETHER_move_entity(ETHER_entity *curr, ETHER_entity *dest_prev);
void ETHER_swap_entities(ETHER_entity *prev, ETHER_entity *next);
void ETHER_swap_entities_adjacent(ETHER_entity *prev, ETHER_entity *next);
void ETHER_swap_entities_nonadjacent(ETHER_entity *prev, ETHER_entity *next);
void ETHER_entities_add(ETHER_state_entities *entities, ETHER_entity *entity);
ETHER_entity *ETHER_entities_pop(ETHER_state_entities *entities);

struct _ETHER_state_textures
{
    SDL_Texture *bricks;
    SDL_Texture *bricks_deep;
    SDL_Texture *bricks_mossy;
    SDL_Texture *bricks_sand;
    SDL_Texture *bricks_stone;
    SDL_Texture *dirt;
    SDL_Texture *dirt_deep;
    SDL_Texture *dirt_gravel;
    SDL_Texture *dirt_nature;
    SDL_Texture *leaves_1;
    SDL_Texture *leaves_2;
    SDL_Texture *slate;
    SDL_Texture *item;
    SDL_Texture *cool_rock;
    SDL_Texture *log_rowanoak_top;
    SDL_Texture *log_rowanoak_side;
    SDL_Texture *log_rowanoak_planks;
    SDL_Texture *log_mystic_top;
    SDL_Texture *log_mystic_side;
    SDL_Texture *grass;
    SDL_Texture *undef;
};

typedef uint8_t block_id_t;

void ETHER_state_textures_load(SDL_Renderer *renderer, ETHER_state_textures *textures);
SDL_Texture *ETHER_blockid_to_texture(ETHER_state_textures *textures, block_id_t id);

#define BLOCK_SIZE 16
#define ENTITY_SIZE 16
#define CHUNK_SIZE 8
#define CHUNK_WORLD_SIZE (BLOCK_SIZE * CHUNK_SIZE)
#define QUADTREE_SIZE 255
#define QUADTREE_DEPTH 8
#define WORLD_SIZE (QUADTREE_SIZE * CHUNK_WORLD_SIZE)

typedef uint8_t chunk_coord_t;
typedef uint8_t quadtree_coord_t;
typedef uint16_t world_coord_t;
typedef struct _ETHER_chunk ETHER_chunk;
typedef union _ETHER_branch ETHER_branch;
typedef struct _ETHER_node ETHER_node;
typedef struct _ETHER_leaf ETHER_leaf;
typedef struct _ETHER_entity_node ETHER_entity_node;
typedef struct _ETHER_bucket ETHER_bucket;
typedef struct _ETHER_array ETHER_array;

struct _ETHER_state_quadtree
{
    ETHER_node *base;
};

struct _ETHER_chunk
{
    block_id_t blocks[CHUNK_SIZE * CHUNK_SIZE];
    SDL_Texture *cache;
    BOOL update_cache;
};

union _ETHER_branch
{
    ETHER_node **quad;
    ETHER_leaf *leaf;
};

struct _ETHER_node
{
    ETHER_node *parent;
    uint8_t ppos;
    uint8_t depth;
    ETHER_rect_u8 rect;
    ETHER_branch branch;
    BOOL is_leaf;
};

struct _ETHER_entity_node
{
    struct _ETHER_entity_node *prev;
    struct _ETHER_entity_node *next;
    ETHER_entity *curr;
};

struct _ETHER_bucket
{
    ETHER_leaf *parent;
    ETHER_entity_node *head;
    ETHER_entity_node *tail;
};

struct _ETHER_leaf
{
    ETHER_node *parent;
    ETHER_rect_u8 rect_quadtree;
    ETHER_rect_u16 rect_world;
    ETHER_chunk chunk;
    ETHER_bucket bucket;
};

void ETHER_buckets_add(ETHER_state_quadtree *quadtree, ETHER_entity *entity);
void ETHER_bucket_add(ETHER_bucket *bucket, ETHER_entity *entity);
void ETHER_bucket_remove(ETHER_bucket *bucket, ETHER_entity *entity);

void ETHER_leaf_create(ETHER_leaf **leaf, ETHER_node *parent);
void ETHER_node_create(ETHER_node **node, ETHER_node *parent, uint8_t ppos);
void ETHER_node_subdivide(ETHER_node *node);
ETHER_leaf *ETHER_node_create_leaf(ETHER_node *node, ETHER_vec2_u8 pos);
BOOL ETHER_node_isend(ETHER_node *node);
uint8_t ETHER_node_get_quadrant(ETHER_node *node, ETHER_vec2_u8 pos);
uint8_t ETHER_node_get_depth(ETHER_node *node);
uint8_t ETHER_node_get_ppos(ETHER_node *node);
ETHER_rect_u8 ETHER_node_get_rect(ETHER_node *node);
ETHER_rect_u16 ETHER_rect_quadtree_to_world(ETHER_rect_u8 rect);
ETHER_rect_u8 ETHER_rect_world_to_quadtree(ETHER_rect_u16 rect);
ETHER_rect_u8 ETHER_rect_world_to_quadtree_2(ETHER_rect_u16 rect);
void ETHER_node_debug(ETHER_node *node);
ETHER_array *ETHER_node_get_rect_leaves(ETHER_node *node, ETHER_rect_u8 rect);
void ETHER_array_debug(ETHER_array *array);

struct _ETHER_array
{
    void **data;
    uint16_t len;
    size_t space;
};

#endif
