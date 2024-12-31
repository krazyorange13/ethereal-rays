#include "utils.h"

BOOL ETHER_rect_u8_iszero(ETHER_rect_u8 rect)
{
    return rect.x == 0
        && rect.y == 0
        && rect.w == 0
        && rect.h == 0;
}

BOOL ETHER_rect_u16_iszero(ETHER_rect_u16 rect)
{
    return rect.x == 0
        && rect.y == 0
        && rect.w == 0
        && rect.h == 0;
}

void ETHER_rect_u8_debug(ETHER_rect_u8 rect)
{
    printf("{ %d %d %d %d }\n", rect.x, rect.y, rect.w, rect.h);
}

void ETHER_rect_u16_debug(ETHER_rect_u16 rect)
{
    printf("{ %d %d %d %d }\n", rect.x, rect.y, rect.w, rect.h);
}

void ETHER_rect_u8_debug_inline(ETHER_rect_u8 rect)
{
    printf("{ %d %d %d %d }", rect.x, rect.y, rect.w, rect.h);
}

void ETHER_rect_u16_debug_inline(ETHER_rect_u16 rect)
{
    printf("{ %d %d %d %d }", rect.x, rect.y, rect.w, rect.h);
}

BOOL ETHER_vec2_in_rect_u8(ETHER_vec2_u8 pos, ETHER_rect_u8 rect)
{
    return pos.x >= rect.x && pos.x <= rect.x + rect.w
        && pos.y >= rect.y && pos.y <= rect.y + rect.h;
}

BOOL ETHER_vec2_in_rect_u16(ETHER_vec2_u16 pos, ETHER_rect_u16 rect)
{
    return pos.x >= rect.x && pos.x <= rect.x + rect.w
        && pos.y >= rect.y && pos.y <= rect.y + rect.h;
}