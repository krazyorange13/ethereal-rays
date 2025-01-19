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

#define LOAD_TEXTURE(var, path) \
    var = IMG_LoadTexture(renderer, path); \
    SDL_SetTextureScaleMode(var, SDL_SCALEMODE_NEAREST);

void ETHER_state_textures_load(SDL_Renderer *renderer, ETHER_state_textures *textures)
{
    LOAD_TEXTURE(textures->bricks, "res/bricks.png")
    LOAD_TEXTURE(textures->bricks_deep, "res/bricks-deep.png")
    LOAD_TEXTURE(textures->bricks_mossy, "res/bricks-mossy.png")
    LOAD_TEXTURE(textures->bricks_sand, "res/bricks-sand.png")
    LOAD_TEXTURE(textures->bricks_stone, "res/bricks-stone.png")
    LOAD_TEXTURE(textures->dirt, "res/dirt.png")
    LOAD_TEXTURE(textures->dirt_deep, "res/dirt-deep.png")
    LOAD_TEXTURE(textures->dirt_gravel, "res/dirt-gravel.png")
    LOAD_TEXTURE(textures->dirt_nature, "res/dirt-nature.png")
    LOAD_TEXTURE(textures->leaves_1, "res/leaves-1.png")
    LOAD_TEXTURE(textures->leaves_2, "res/leaves-2.png")
    LOAD_TEXTURE(textures->slate, "res/slate.png")
    LOAD_TEXTURE(textures->cool_rock, "res/some-cool-stone.png")
    LOAD_TEXTURE(textures->item, "res/item.png")
    LOAD_TEXTURE(textures->log_rowanoak_top, "res/log-rowanoak-top.png")
    LOAD_TEXTURE(textures->log_rowanoak_side, "res/log-rowanoak-side.png")
    LOAD_TEXTURE(textures->log_rowanoak_planks, "res/log-rowanoak-planks.png")
    LOAD_TEXTURE(textures->log_mystic_top, "res/log-mystic-top.png")
    LOAD_TEXTURE(textures->log_mystic_side, "res/log-mystic-side.png")
    LOAD_TEXTURE(textures->grass, "res/grass.png")
    LOAD_TEXTURE(textures->undef, "res/undef.png")
}

SDL_Texture *ETHER_blockid_to_texture(ETHER_state_textures *textures, ETHER_block_tex_id_t id)
{
    switch (id)
    {
    case 0: return textures->bricks;
    case 1: return textures->bricks_deep;
    case 2: return textures->bricks_mossy;
    case 3: return textures->bricks_sand;
    case 4: return textures->bricks_stone;
    case 5: return textures->dirt;
    case 6: return textures->dirt_deep;
    case 7: return textures->dirt_gravel;
    case 8: return textures->dirt_nature;
    case 9: return textures->leaves_1;
    case 10: return textures->leaves_2;
    case 11: return textures->slate;
    case 12: return textures->item;
    case 13: return textures->cool_rock;
    case 14: return textures->log_rowanoak_top;
    case 15: return textures->log_rowanoak_side;
    case 16: return textures->log_rowanoak_planks;
    case 17: return textures->log_mystic_top;
    case 18: return textures->log_mystic_side;
    case 19: return textures->grass;
    default: return textures->undef;
    }
}
