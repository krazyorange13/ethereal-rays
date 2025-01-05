#ifndef _ETHER_UTILS_H
#define _ETHER_UTILS_H

#define _GNU_SOURCE
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"

#include "SDL3/SDL.h"

typedef uint8_t BOOL;
#define FALSE 0
#define TRUE 1

#define CEIL_POS(X) ((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))
#define CEIL_NEG(X) (int)(X)
#define CEIL(X) (((X) > 0) ? CEIL_POS(X) : CEIL_NEG(X))
#define ROUND(X) (((X) < (int)(X) + 0.5) ? (int)(X) : (int)(X) + 1)
#define FLOOR(X) ((int)(X))
#define NUFFIN(X) (X)
#define ABS(X) (((X) < 0) ? -(X) : (X))
#define SIGN(X) (((X) < 0) ? -1 : 1)
#define SIGN2(X) (((X) == 0) ? 0 : (((X) < 0) ? -1 : 1))
#define CLAMP(X, MIN, MAX) ((((X) < (MIN)) ? (MIN) : (X)) > (MAX) ? (MAX) : (X))

typedef struct _ETHER_vec2_u8 ETHER_vec2_u8;
typedef struct _ETHER_vec2_u16 ETHER_vec2_u16;
typedef struct _ETHER_rect_u8 ETHER_rect_u8;
typedef struct _ETHER_rect_u16 ETHER_rect_u16;
typedef struct _ETHER_vec2_float ETHER_vec2_float;
typedef struct _ETHER_col ETHER_col;

struct _ETHER_vec2_u8
{
    uint8_t x;
    uint8_t y;
};

struct _ETHER_vec2_u16
{
    uint16_t x;
    uint16_t y;
};

struct _ETHER_rect_u8
{
    uint8_t x;
    uint8_t y;
    uint8_t w;
    uint8_t h;
};

struct _ETHER_rect_u16
{
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
};

BOOL ETHER_rect_u8_iszero(ETHER_rect_u8 rect);
BOOL ETHER_rect_u16_iszero(ETHER_rect_u16 rect);
BOOL ETHER_rects_equal_u16(ETHER_rect_u16 a, ETHER_rect_u16 b);
void ETHER_rect_u8_debug(ETHER_rect_u8 rect);
void ETHER_rect_u16_debug(ETHER_rect_u16 rect);
void ETHER_rect_u8_debug_inline(ETHER_rect_u8 rect);
void ETHER_rect_u16_debug_inline(ETHER_rect_u16 rect);
BOOL ETHER_vec2_in_rect_u8(ETHER_vec2_u8 pos, ETHER_rect_u8 rect);
BOOL ETHER_vec2_in_rect_u16(ETHER_vec2_u16 pos, ETHER_rect_u16 rect);
BOOL ETHER_rects_collide_u8(ETHER_rect_u8 a, ETHER_rect_u8 b);
BOOL ETHER_rects_collide_u16(ETHER_rect_u16 a, ETHER_rect_u16 b);
BOOL ETHER_rects_collide_u8_2(ETHER_rect_u8 a, ETHER_rect_u8 b);
BOOL ETHER_rects_collide_u16_2(ETHER_rect_u16 a, ETHER_rect_u16 b);
SDL_FRect ETHER_rect_u16_to_sdl(ETHER_rect_u16 rect);

struct _ETHER_vec2_float
{
    float x;
    float y;
};

struct _ETHER_col
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

#endif
