#ifndef _ETHER_STATE_H
#define _ETHER_STATE_H

#include "utils.h"

#include "SDL3/SDL.h"

typedef struct _ETHER_state ETHER_state;
typedef struct _ETHER_state_input ETHER_state_input;
typedef struct _ETHER_state_keybinds ETHER_state_keybinds;
typedef struct _ETHER_state_textures ETHER_state_textures;
typedef struct _ETHER_state_entities ETHER_state_entities;
typedef struct _ETHER_block_chunks ETHER_state_chunks;

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
    ETHER_state_chunks *chunks;

    uint16_t player;
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

typedef uint8_t ETHER_block_tex_id_t;

void ETHER_state_textures_load(SDL_Renderer *renderer, ETHER_state_textures *textures);
SDL_Texture *ETHER_blockid_to_texture(ETHER_state_textures *textures, ETHER_block_tex_id_t id);

/*
 * TABLES
 */

#define ETHER_TABLE_MAX UINT16_MAX
typedef uint16_t ETHER_table_id_t;

/* ENTITIES */

#define ETHER_STATE_ENTITIES_MAX ETHER_TABLE_MAX
#define ETHER_ENTITY_SIZE 16

typedef ETHER_table_id_t ETHER_entity_id_t;
typedef ETHER_rect_u16 ETHER_rect_world_space;

typedef struct _ETHER_state_entities ETHER_state_entities;

struct _ETHER_state_entities
{
    // spatial structure
    struct _ETHER_entity_chunks *chunks;

    ETHER_entity_id_t len;
    ETHER_entity_id_t cap;

    ETHER_rect_world_space *rects;
};

/* BLOCKS */

#define ETHER_STATE_CHUNKS_MAX ETHER_TABLE_MAX
typedef uint16_t ETHER_chunk_id_t;

typedef struct _ETHER_block_chunks ETHER_state_chunks;

/*
 * SPACIAL STRUCTURES
 */

/* ENTITIES */

#define _world_units

#define ETHER_ENTITY_CHUNK_CELL_SIZE 32 // entity_chunk_cell size in pixels
#define ETHER_ENTITY_CHUNK_SIZE_CELLS 8 // entity_chunk size in cells (ONE SIDE)
#define ETHER_ENTITY_CHUNK_SIZE_WORLD (ETHER_ENTITY_CHUNK_SIZE_CELLS * ETHER_ENTITY_CHUNK_CELL_SIZE) // entity_chunk size in pixels
#define ETHER_ENTITY_CHUNK_CELL_CAP 10

typedef ETHER_rect_u8 ETHER_rect_entity_chunk_space;
typedef uint16_t ETHER_entity_chunk_id_t;
typedef uint16_t ETHER_entity_chunk_cell_id_t;

typedef struct _ETHER_entity_chunks ETHER_entity_chunks;
typedef struct _ETHER_entity_chunk ETHER_entity_chunk;
typedef struct _ETHER_entity_chunk_cell ETHER_entity_chunk_cell;

struct _ETHER_entity_chunks
{
    ETHER_entity_chunk_id_t len;
    ETHER_entity_chunk_id_t cap;

    ETHER_rect_entity_chunk_space *rects;
    ETHER_entity_chunk *chunks;
};


struct _ETHER_entity_chunk_cell
{
    uint8_t len; // technically, to be consistent, this should have some crazy typedef but i dont want to do that
    ETHER_entity_id_t entities[ETHER_ENTITY_CHUNK_CELL_CAP];
};

struct _ETHER_entity_chunk
{
    // ETHER_entity_id_t entities[SQUARE(ETHER_ENTITY_CHUNK_SIZE_CELLS) * ETHER_ENTITY_CHUNK_CELL_CAP];
    ETHER_entity_chunk_cell cells[SQUARE(ETHER_ENTITY_CHUNK_SIZE_CELLS)];
};

/* BLOCKS */

#define ETHER_BLOCK_CHUNK_SIZE 8
#define ETHER_BLOCK_SIZE 16
#define ETHER_BLOCK_CHUNK_WORLD_SIZE (ETHER_BLOCK_CHUNK_SIZE * ETHER_BLOCK_SIZE)

#define FLAG_DIRTY TRUE
#define FLAG_CLEAN FALSE

typedef ETHER_rect_u8 ETHER_rect_block_chunk_space;

typedef struct _ETHER_block_chunks ETHER_block_chunks;
typedef struct _ETHER_block_chunk ETHER_block_chunk;
typedef struct _ETHER_block ETHER_block;

struct _ETHER_block_chunks
{
    ETHER_chunk_id_t len;
    ETHER_chunk_id_t cap;

    ETHER_rect_block_chunk_space *rects;
    ETHER_block_chunk *chunks;
    BOOL *render_cache_flags;
    SDL_Texture **render_caches;
};

struct _ETHER_block
{
    ETHER_block_tex_id_t tex;
};

struct _ETHER_block_chunk
{
    ETHER_block blocks[SQUARE(ETHER_BLOCK_CHUNK_SIZE)];
};

#endif
