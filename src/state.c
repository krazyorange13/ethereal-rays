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