#include "state.h"
#include "settings.h"

#include "SDL3_image/SDL_image.h"

ETHER_state_keybinds ETHER_keybinds_get_default()
{
    ETHER_state_keybinds state;
    state.move_up = STATE_DEFAULT_KEYBINDS_MOVE_UP;
    state.move_down = STATE_DEFAULT_KEYBINDS_MOVE_DOWN;
    state.move_left = STATE_DEFAULT_KEYBINDS_MOVE_LEFT;
    state.move_right = STATE_DEFAULT_KEYBINDS_MOVE_RIGHT;
    state.smth = STATE_DEFAULT_KEYBINDS_SMTH;
    return state;
}
