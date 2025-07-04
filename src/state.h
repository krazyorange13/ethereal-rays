#ifndef _ETHER_STATE_H
#define _ETHER_STATE_H

#include "utils.h"

#include "SDL3/SDL.h"

typedef struct _ETHER_state ETHER_state;
typedef struct _ETHER_state_input ETHER_state_input;
typedef struct _ETHER_state_keybinds ETHER_state_keybinds;
typedef struct _ETHER_state_entities ETHER_state_entities;
typedef struct _ETHER_block_chunks ETHER_state_chunks;
typedef struct _ETHER_texture_sheets ETHER_state_textures;
typedef struct _ETHER_animation_table ETHER_state_animations;

ETHER_state_keybinds ETHER_keybinds_get_default();

struct _ETHER_state
{
    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    ETHER_vec2_float mouse;
    ETHER_state_input *input;
    ETHER_state_keybinds *keybinds;
    ETHER_state_entities *entities;
    ETHER_state_chunks *chunks;
    ETHER_state_textures *textures;
    ETHER_state_animations *animations;
    double fps;
    BOOL quit;
    BOOL smth;
    uint64_t frames;
    ETHER_rect_s16 camera;
};

struct _ETHER_state_input
{
    BOOL move_up;
    BOOL move_down;
    BOOL move_left;
    BOOL move_right;
    BOOL smth;
};

struct _ETHER_state_keybinds
{
    SDL_Scancode move_up;
    SDL_Scancode move_down;
    SDL_Scancode move_right;
    SDL_Scancode move_left;
    SDL_Scancode smth;
};

/*
 * TABLES
 */

#define ETHER_TABLE_MAX UINT16_MAX
typedef uint16_t ETHER_table_id_t;

/* ENTITIES */

// TODO: BENCHMARK with and without this value.
// uses less memory, but may be slower.
// this is for all the enums. (try with structs too?)
// #define ETHER_DEBUG_PACKED
#ifdef ETHER_DEBUG_PACKED
#define ETHER__packed__ __attribute__((__packed__))
#else
#define ETHER__packed__
#endif

#define ETHER_STATE_ENTITIES_MAX ETHER_TABLE_MAX
#define ETHER_ENTITY_SIZE 16

typedef ETHER_table_id_t ETHER_entity_id_t;
typedef enum ETHER_entity_type_t ETHER_entity_type_t;
typedef enum ETHER_entity_behavior_t ETHER_entity_behavior_t;
typedef enum ETHER_entity_direction_t ETHER_entity_direction_t;

typedef ETHER_rect_u16 ETHER_rect_world_space;

typedef struct _ETHER_state_entities ETHER_state_entities;
typedef struct _ETHER_entity_state ETHER_entity_state;

struct _ETHER_state_entities
{
    // spatial structure
    struct _ETHER_entity_chunks *chunks;

    ETHER_entity_id_t len;
    ETHER_entity_id_t cap;

    ETHER_rect_world_space *rects;
    ETHER_rect_s16 *velocities;
    // TODO: add a velocity structure too
    // decent collision handling is impossible
    // without it unfortunately
    ETHER_entity_state *states;
    ETHER_entity_type_t *types;
    // i smell a switchy switchy function coming up here
};

enum ETHER__packed__ ETHER_entity_type_t
{
    ETHER_ENTITY_TYPE_NULL,
    ETHER_ENTITY_TYPE_PERSON,
    ETHER_ENTITY_TYPE_PLAYER,
};


// haha f*ck the brits
enum ETHER__packed__ ETHER_entity_behavior_t
{
    ETHER_ENTITY_BEHAVIOR_NULL,
    ETHER_ENTITY_BEHAVIOR_PERSON_IDLE,
    ETHER_ENTITY_BEHAVIOR_PERSON_WALK,
    // question: use entity-generic types like
    // ETHER_ENTITY_BEHAVIOR_IDLE? they should be able to be
    // used enough, and keep the enum smaller.
};

enum ETHER__packed__ ETHER_entity_direction_t
{
    ETHER_ENTITY_DIRECTION_LEFT,
    ETHER_ENTITY_DIRECTION_RIGHT,
};

// this struct is 8 bytes (unpacked).
// this is a lot larger than it needs to be (2 bytes)
// but 8 is also just the size of a pointer, and
// an x64 cpu can copy this whole thing in one move.
struct ETHER__packed__ _ETHER_entity_state
{
    ETHER_entity_behavior_t behavior;
    ETHER_entity_direction_t direction;
    // uint8_t direction;
    // // using this makes no difference in padded struct size
    // // (order didn't change anything either)
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
#define ETHER_ENTITY_CHUNK_CELL_CAP 64

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
    uint8_t tex;
};

struct _ETHER_block_chunk
{
    ETHER_block blocks[SQUARE(ETHER_BLOCK_CHUNK_SIZE)];
};

/* TEXTURES */

// each entity will have three, four frame general animations:
// IDLE, WALK, and ATTACK. but thats a lot of memory. each entity
// needs to be as lightweight as possible.
// SOLUTION: each entity has an (uint8_t) ETHER_entity_type_id_t member.
// there is a table of defining each entity type's idle, walk, and
// attack animations, since entities won't have custom animations going on.

// controlling animations: this is a toughie, but we can kill two birds with
// one stone and provide a base for future behaviour control and decision trees,
// not just animations. keep a variable, like (uint8_t) ETHER_entity_state_id_t
// that has an animation tied to it, also saved somewhere with the constant
// ETHER_entity_type_id_t table

// what about simple horizontal flip movement? rect tracking is too much,
// we could have a simple byte for that but it might be better to smush it
// into ETHER_entity_state_id_t

typedef struct _ETHER_animation ETHER_animation;

// unfortunately, this makes bad things happen to those who don't deserve them.
struct _ETHER_animation
{
    uint8_t index;
    uint8_t frames;
    // uint8_t start;
    // uint8_t end;
    uint8_t speed;
    uint8_t sheet; // sheet poggers?
    // use switch statement helper function sh*t, or
    // create multiple structures? all of them will 
    // probably be pretty much the same.
};
// this is a happy 4 byte structure :)

typedef struct _ETHER_animation_table ETHER_animation_table;

// TODO: rename all `_struct`s to just `struct`
// because technically they're invalid or smth lmao

struct _ETHER_animation_table
{
    ETHER_animation person_idle;
    ETHER_animation person_walk;
    // i hate having to keep this updated but whatever
    // f*ck me in the a** i guess
};

#define ETHER_TEXTURE_BLOCK_WIDTH  16
#define ETHER_TEXTURE_BLOCK_HEIGHT 16
#define ETHER_TEXTURE_ENTITY_WIDTH  16
#define ETHER_TEXTURE_ENTITY_HEIGHT 32 // this is the banger!
#define ETHER_TEXTURE_ITEM_WIDTH  16
#define ETHER_TEXTURE_ITEM_HEIGHT 16
#define ETHER_TEXTURE_EFFECT_WIDTH  16
#define ETHER_TEXTURE_EFFECT_HEIGHT 16
#define ETHER_TEXTURE_UI_WIDTH  16
#define ETHER_TEXTURE_UI_HEIGHT 16

typedef enum ETHER_texture_sheet_t ETHER_texture_sheet_t;

enum ETHER_texture_sheet_t
{
    ETHER_TEXTURE_SHEET_BLOCK,
    ETHER_TEXTURE_SHEET_ENTITY,
    ETHER_TEXTURE_SHEET_ITEM,
    ETHER_TEXTURE_SHEET_EFFECT,
    ETHER_TEXTURE_SHEET_UI,
};

#define ETHER_TEXTURE_SHEET_WIDTH_SPRITES  8
#define ETHER_TEXTURE_SHEET_HEIGHT_SPRITES 16

#define ETHER_TEXTURE_SHEET_PATH_BLOCK  "/Users/teol/Documents/hac2/ethereal-rays/res/block.png"
#define ETHER_TEXTURE_SHEET_PATH_ENTITY "/Users/teol/Documents/hac2/ethereal-rays/res/entity.png"
#define ETHER_TEXTURE_SHEET_PATH_ITEM   "/Users/teol/Documents/hac2/ethereal-rays/res/item.png"
#define ETHER_TEXTURE_SHEET_PATH_EFFECT "/Users/teol/Documents/hac2/ethereal-rays/res/effect.png"
#define ETHER_TEXTURE_SHEET_PATH_UI     "/Users/teol/Documents/hac2/ethereal-rays/res/ui.png"

typedef struct _ETHER_texture_sheets ETHER_texture_sheets;

struct _ETHER_texture_sheets
{
    SDL_Texture *block;
    SDL_Texture *entity;
    SDL_Texture *item;
    SDL_Texture *effect;
    SDL_Texture *ui;
};

enum ETHER_texture_block_t
{
    ETHER_TEXTURE_BLOCK_NULL,
    ETHER_TEXTURE_BLOCK_BRICKS,
    ETHER_TEXTURE_BLOCK_BRICKS_STONE,
    ETHER_TEXTURE_BLOCK_BRICKS_DEEP,
    ETHER_TEXTURE_BLOCK_BRICKS_RED,
    ETHER_TEXTURE_BLOCK_BRICKS_GOLDEN,
    ETHER_TEXTURE_BLOCK_BRICKS_ICE,
    ETHER_TEXTURE_BLOCK_BRICKS_MYSTIC,
};

enum ETHER_texture_entity_t
{
    ETHER_TEXTURE_ENTITY_NULL,
    ETHER_TEXTURE_ENTITY_PERSON_IDLE_0 = 1,
    ETHER_TEXTURE_ENTITY_PERSON_IDLE_1,
    ETHER_TEXTURE_ENTITY_PERSON_IDLE_2,
    ETHER_TEXTURE_ENTITY_PERSON_IDLE_3,
    ETHER_TEXTURE_ENTITY_PERSON_WALK_0 = 9,
    ETHER_TEXTURE_ENTITY_PERSON_WALK_1,
    ETHER_TEXTURE_ENTITY_PERSON_WALK_2,
    ETHER_TEXTURE_ENTITY_PERSON_WALK_3,
};

enum ETHER_texture_item_t
{
    ETHER_TEXTURE_ITEM_NULL
};

enum ETHER_texture_effect_t
{
    ETHER_TEXTURE_EFFECT_NULL
};

enum ETHER_texture_ui_t
{
    ETHER_TEXTURE_UI_NULL
};

typedef ETHER_rect_u16 ETHER_rect_texture;

typedef enum ETHER_texture_block_t ETHER_texture_block_t;
typedef enum ETHER_texture_entity_t ETHER_texture_entity_t;
typedef enum ETHER_texture_item_t ETHER_texture_item_t;
typedef enum ETHER_texture_effect_t ETHER_texture_effect_t;
typedef enum ETHER_texture_ui_t ETHER_texture_ui_t;

static inline SDL_FRect ETHER_rect_texture_to_sdl(ETHER_rect_texture rect)
{
    return (SDL_FRect) { rect.x, rect.y, rect.w, rect.h };
}

static inline ETHER_rect_texture ETHER_texture_get_block(ETHER_texture_block_t id)
{
    return (ETHER_rect_texture) { 
        (id % ETHER_TEXTURE_SHEET_WIDTH_SPRITES) * ETHER_TEXTURE_BLOCK_WIDTH,
        (id / ETHER_TEXTURE_SHEET_WIDTH_SPRITES) * ETHER_TEXTURE_BLOCK_HEIGHT,
        ETHER_TEXTURE_BLOCK_WIDTH,
        ETHER_TEXTURE_BLOCK_HEIGHT
    };
}

static inline ETHER_rect_texture ETHER_texture_get_entity(ETHER_texture_entity_t id)
{

    return (ETHER_rect_texture) { 
        (id % ETHER_TEXTURE_SHEET_WIDTH_SPRITES) * ETHER_TEXTURE_ENTITY_WIDTH,
        (id / ETHER_TEXTURE_SHEET_WIDTH_SPRITES) * ETHER_TEXTURE_ENTITY_HEIGHT,
        ETHER_TEXTURE_ENTITY_WIDTH,
        ETHER_TEXTURE_ENTITY_HEIGHT
    };
}

static inline ETHER_rect_texture ETHER_texture_get_item(ETHER_texture_item_t id)
{
    return (ETHER_rect_texture) { 
        (id % ETHER_TEXTURE_SHEET_WIDTH_SPRITES) * ETHER_TEXTURE_ITEM_WIDTH,
        (id / ETHER_TEXTURE_SHEET_WIDTH_SPRITES) * ETHER_TEXTURE_ITEM_HEIGHT,
        ETHER_TEXTURE_ITEM_WIDTH,
        ETHER_TEXTURE_ITEM_HEIGHT
    };
}
static inline ETHER_rect_texture ETHER_texture_get_effect(ETHER_texture_effect_t id)
{
    return (ETHER_rect_texture) { 
        (id % ETHER_TEXTURE_SHEET_WIDTH_SPRITES) * ETHER_TEXTURE_EFFECT_WIDTH,
        (id / ETHER_TEXTURE_SHEET_WIDTH_SPRITES) * ETHER_TEXTURE_EFFECT_HEIGHT,
        ETHER_TEXTURE_EFFECT_WIDTH,
        ETHER_TEXTURE_EFFECT_HEIGHT
    };
}
static inline ETHER_rect_texture ETHER_texture_get_ui(ETHER_texture_ui_t id)
{
    return (ETHER_rect_texture) { 
        (id % ETHER_TEXTURE_SHEET_WIDTH_SPRITES) * ETHER_TEXTURE_UI_WIDTH,
        (id / ETHER_TEXTURE_SHEET_WIDTH_SPRITES) * ETHER_TEXTURE_UI_HEIGHT,
        ETHER_TEXTURE_UI_WIDTH,
        ETHER_TEXTURE_UI_HEIGHT
    };
}

#endif
