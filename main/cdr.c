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

#include <hdbg_cdr.h>

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include "lua.h"
#include "lauxlib.h"

float cdr_speedfactor = 1.0f;

extern lua_State *L;

void cdr_init(void)
{
    int ok;
    lua_getglobal(L, "config");

    lua_getfield(L, -1, "cdrspeed");
    cdr_speedfactor = lua_tonumberx(L, -1, &ok);
    if (!ok) {
        fprintf(
            stderr,
            "Error in config: Bad cdrspeed (not a number)\n"
        );
        exit(EXIT_FAILURE);
    }
    lua_pop(L, 1);

    lua_pop(L, 1);
}

void cdr_quit(void)
{
}
