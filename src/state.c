#include "state.h"
#include "settings.h"

ETHER_state_keybinds create_default_state_keybinds()
{
    ETHER_state_keybinds s;
    s.move_up = STATE_DEFAULT_KEYBINDS_MOVE_UP;
    s.move_down = STATE_DEFAULT_KEYBINDS_MOVE_DOWN;
    s.move_left = STATE_DEFAULT_KEYBINDS_MOVE_LEFT;
    s.move_right = STATE_DEFAULT_KEYBINDS_MOVE_RIGHT;
    return s;
}
