//
// PCSX-HDBG - PCSX-based hack debugger
// Copyright (C) 2019-2021  "chekwob" <chek@wobbyworks.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <hdbg_common.h>
#include <hdbg_pad.h>

#include "lua.h"
#include "lauxlib.h"

const char *pad_buttonnames[] = {
    "select",
    "l3",
    "r3",
    "start",
    "up",
    "right",
    "down",
    "left",
    "l2",
    "r2",
    "l1",
    "r1",
    "triangle",
    "circle",
    "x",
    "square",
    NULL
};

static uint16_t pad_keybuttons = 0xFFFF;
static uint16_t pad_joybuttons = 0xFFFF;
static uint16_t pad_bindings[SDL_NUM_SCANCODES];

static uint8_t pad_analoglx = 0x80;
static uint8_t pad_analogly = 0x80;
static uint8_t pad_analogrx = 0x80;
static uint8_t pad_analogry = 0x80;

static bool pad_forcedigital = false;

static SDL_GameController *pad_joystick = NULL;

static int scr_forcedigitalpad(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    pad_forcedigital = lua_toboolean(L, 1);
    return 0;
}

extern lua_State *L;

#define DEFINE_LUA_FUNCTION(name) \
    lua_pushcfunction(L, scr_##name); \
    lua_setglobal(L, #name);

void pad_init(void)
{
    DEFINE_LUA_FUNCTION(forcedigitalpad);

    for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
        pad_bindings[i] = -1;
    }

    lua_getglobal(L, "config");

    lua_getfield(L, -1, "keymap");
    int t_type = lua_type(L, -1);
    if (t_type != LUA_TTABLE) {
        fprintf(
            stderr,
            "Error in config: Bad keymap (was %s, expected table)\n",
            lua_typename(L, t_type)
        );
        exit(EXIT_FAILURE);
    }

    lua_pushnil(L);
    while (lua_next(L, -2)) {
        int k_type = lua_type(L, -2);
        int v_type = lua_type(L, -1);
        if (k_type != LUA_TSTRING) {
            fprintf(
                stderr,
                "Error in config: Bad keymap key (was %s, expected string)\n",
                lua_typename(L, k_type)
            );
            lua_pop(L, 1);
            continue;
        }
        if (v_type != LUA_TSTRING) {
            fprintf(
                stderr,
                "Error in config: Bad keymap value (was %s, expected string)\n",
                lua_typename(L, v_type)
            );
            lua_pop(L, 1);
            continue;
        }
        const char *k = lua_tostring(L, -2);
        const char *v = lua_tostring(L, -1);

        SDL_Keycode keycode = SDL_GetKeyFromName(k);
        if (keycode == SDLK_UNKNOWN) {
            fprintf(
                stderr,
                "Error in config: Unrecognized keymap key '%s'.\n",
                k
            );
            lua_pop(L, 1);
            continue;
        }

        SDL_Scancode scancode = SDL_GetScancodeFromKey(keycode);
        if (scancode == SDL_SCANCODE_UNKNOWN) {
            fprintf(
                stderr,
                "Error in config: Keymap key '%s' has no matching scancode.\n",
                k
            );
            lua_pop(L, 1);
            continue;
        }

        int button = pad_lookupname(v);
        if (button == -1) {
            fprintf(
                stderr,
                "Error in config: Unrecognized keymap value '%s'.\n",
                v
            );
            lua_pop(L, 1);
            continue;
        }

        pad_bindings[scancode] = button;

        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "gamecontrollerdb");
    t_type = lua_type(L, -1);
    if (t_type != LUA_TSTRING) {
        fprintf(
            stderr,
            "Error in config: Bad gamecontrollerdb (was %s, expected string)\n",
            lua_typename(L, t_type)
        );
        exit(EXIT_FAILURE);
    }
    const char *str = lua_tolstring(L, -1, NULL);
    int err = SDL_GameControllerAddMapping(str);
    if (err == -1) {
        fprintf(
            stderr,
            "SDL_GameControllerAddMapping failed: %s\n",
            SDL_GetError()
        );
        exit(EXIT_FAILURE);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "joyid");
    t_type = lua_type(L, -1);
    if (t_type != LUA_TNIL) {
        int isnum;
        lua_Integer joyid = lua_tointegerx(L, -1, &isnum);
        if (!isnum) {
            fprintf(
                stderr,
                "Error in config: Bad joyid (was %s, expected integer)\n",
                lua_typename(L, t_type)
            );
            exit(EXIT_FAILURE);
        }

        int err = SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
        if (err) {
            fprintf(
                stderr,
                "SDL_InitSubSystem for game controllers failed: %s\n",
                SDL_GetError()
            );
            exit(EXIT_FAILURE);
        }

        int joystick_count = SDL_NumJoysticks();
        if (joystick_count < 0) {
            fprintf(
                stderr,
                "SDL_NumJoysticks failed: %s\n",
                SDL_GetError()
            );
            SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
            exit(EXIT_FAILURE);
        }

        printf("Joysticks available:\n");
        printf("------------------------\n");

        for (int i = 0; i < joystick_count; i++) {
            SDL_GameController *joystick = SDL_GameControllerOpen(i);
            if (!joystick) {
                fprintf(
                    stderr,
                    "SDL_OpenGameController failed: %s\n",
                    SDL_GetError()
                );
                continue;
            }

            if (joyid != -1 && joyid == i) {
                pad_joystick = joystick;
                printf("--> ");
            } else {
                printf("    ");
            }

            char guid[100];
            SDL_JoystickGetGUIDString(
                SDL_JoystickGetGUID(
                    SDL_GameControllerGetJoystick(joystick)
                ),
                guid,
                sizeof(guid)
            );

            printf("%2d: %s [%s]\n", i, SDL_GameControllerName(joystick), guid);

            if (pad_joystick != joystick) {
                SDL_GameControllerClose(joystick);
            }
        }

        printf("------------------------\n");

        if (!pad_joystick) {
            printf("No joystick was selected.\n");
            SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
        }
    } else if (t_type != LUA_TNIL) {
        fprintf(
            stderr,
            "Error in config: Bad joyid value (was %s, expected string)\n",
            lua_typename(L, t_type)
        );
        exit(EXIT_FAILURE);
    }
    lua_pop(L, 1);

    lua_pop(L, 1);
}

void pad_quit(void)
{
    if (pad_joystick) {
        SDL_GameControllerClose(pad_joystick);
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
        pad_joystick = NULL;
    }
}

uint16_t pad_getbuttons(void)
{
    return pad_keybuttons & pad_joybuttons;
}

void pad_getanalogs(uint8_t *lx, uint8_t *ly, uint8_t *rx, uint8_t *ry)
{
    *lx = pad_analoglx;
    *ly = pad_analogly;
    *rx = pad_analogrx;
    *ry = pad_analogry;
}

bool pad_hasanalogs(void)
{
    return !pad_forcedigital;
}

void pad_handlekey(SDL_Scancode scancode, int down)
{
    int button = pad_bindings[scancode];
    if (button == -1)
        return;

    uint16_t buttonmask = 1 << button;

    if (down) {
        pad_keybuttons &= ~buttonmask;
    } else {
        pad_keybuttons |= buttonmask;
    }
}

void pad_handlejbutton(int jbutton, int down)
{
    int button;
    switch (jbutton) {

    case SDL_CONTROLLER_BUTTON_A:
        button = PAD_BUTTON_X;
        break;
    case SDL_CONTROLLER_BUTTON_B:
        button = PAD_BUTTON_CIRCLE;
        break;
    case SDL_CONTROLLER_BUTTON_X:
        button = PAD_BUTTON_SQUARE;
        break;
    case SDL_CONTROLLER_BUTTON_Y:
        button = PAD_BUTTON_TRIANGLE;
        break;

    case SDL_CONTROLLER_BUTTON_BACK:
        button = PAD_BUTTON_SELECT;
        break;
    case SDL_CONTROLLER_BUTTON_START:
        button = PAD_BUTTON_START;
        break;

    case SDL_CONTROLLER_BUTTON_LEFTSTICK:
        button = PAD_BUTTON_L3;
        break;
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
        button = PAD_BUTTON_R3;
        break;

    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        button = PAD_BUTTON_L1;
        break;
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        button = PAD_BUTTON_R1;
        break;

    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        button = PAD_BUTTON_UP;
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        button = PAD_BUTTON_DOWN;
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        button = PAD_BUTTON_LEFT;
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        button = PAD_BUTTON_RIGHT;
        break;

    default:
        return;

    }

    uint16_t buttonmask = 1 << button;

    if (down) {
        pad_joybuttons &= ~buttonmask;
    } else {
        pad_joybuttons |= buttonmask;
    }
}

void pad_handlejaxis(int jaxis, int position)
{
    uint16_t value = position;
    value += 0x8000;
    value >>= 8;

    switch (jaxis) {

    case SDL_CONTROLLER_AXIS_LEFTX:
        pad_analoglx = value;
        break;

    case SDL_CONTROLLER_AXIS_LEFTY:
        pad_analogly = value;
        break;

    case SDL_CONTROLLER_AXIS_RIGHTX:
        pad_analogrx = value;
        break;

    case SDL_CONTROLLER_AXIS_RIGHTY:
        pad_analogry = value;
        break;

    case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: {
        uint64_t buttonmask;
        if (jaxis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) {
            buttonmask = 1 << PAD_BUTTON_L2;
        } else {
            buttonmask = 1 << PAD_BUTTON_R2;
        }
        if (position >= 32767 / 3) {
            pad_joybuttons &= ~buttonmask;
        } else {
            pad_joybuttons |= buttonmask;
        }
        break;
    }

    }
}

void pad_clearkeys(void)
{
    pad_keybuttons = 0xFFFF;
}

int pad_lookupname(const char *name)
{
    for (int i = 0; pad_buttonnames[i]; i++) {
        if (strcmp(name, pad_buttonnames[i]) == 0) {
            return i;
        }
    }
    return -1;
}
