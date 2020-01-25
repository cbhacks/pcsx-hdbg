//
// PCSX-HDBG - PCSX-based hack debugger
// Copyright (C) 2019-2020  "chekwob" <chek@wobbyworks.com>
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

#include <hdbg_pad.h>

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include "lua.h"

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

static uint16_t pad_buttons = 0xFFFF;
static uint16_t pad_bindings[SDL_NUM_SCANCODES];

extern lua_State *L;

void pad_init(void)
{
    for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
        pad_bindings[i] = -1;
    }

    lua_getglobal(L, "config");
    lua_getfield(L, -1, "keymap");
    lua_remove(L, -2);
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
}

void pad_quit(void)
{
}

uint16_t pad_getbuttons(void)
{
    return pad_buttons;
}

void pad_handlekey(SDL_Scancode scancode, int down)
{
    int button = pad_bindings[scancode];
    if (button == -1)
        return;

    uint16_t buttonmask = 1 << button;

    if (down) {
        pad_buttons &= ~buttonmask;
    } else {
        pad_buttons |= buttonmask;
    }
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
