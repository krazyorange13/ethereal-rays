#ifndef _ETHER_UTILS_H
#define _ETHER_UTILS_H

#define _GNU_SOURCE
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"

typedef uint8_t BOOL;
#define FALSE 0
#define TRUE 1

typedef struct _ETHER_vec2 ETHER_vec2;
typedef struct _ETHER_rect ETHER_rect;
typedef struct _ETHER_col ETHER_col;

struct _ETHER_vec2
{
    uint16_t x;
    uint16_t y;
};

struct _ETHER_rect
{
    ETHER_vec2 pos;
    ETHER_vec2 dim;
};

struct _ETHER_col
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

#endif