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
#include <hdbg_trap.h>

#include "lua.h"
#include "lauxlib.h"

#include "../core/psxcommon.h"

struct trap {
    uint32_t addr;
    int func;
    struct trap *next;
};

struct trap *trap_chains[0x10000];

static int scr_trapexec(lua_State *L)
{
    uint32_t addr = luaL_checkinteger(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    lua_pushvalue(L, 2);
    int func = luaL_ref(L, LUA_REGISTRYINDEX);

    struct trap *new_trap = malloc(sizeof(struct trap));
    if (!new_trap) {
        return luaL_error(L, "failed to allocate memory for trap");
    }

    new_trap->addr = addr;
    new_trap->func = func;
    new_trap->next = trap_chains[TRAP_HASH(addr)];
    trap_chains[TRAP_HASH(addr)] = new_trap;
    return 0;
}

extern lua_State *L;

#define DEFINE_LUA_FUNCTION(name) \
    lua_pushcfunction(L, scr_##name); \
    lua_setglobal(L, #name);

void trap_init(void)
{
    DEFINE_LUA_FUNCTION(trapexec);
}

void trap_quit(void)
{
}

void trap_raise(uint32_t addr)
{
    for (struct trap *it = trap_chains[TRAP_HASH(addr)]; it; it = it->next) {
        if (it->addr != addr)
            continue;

        lua_rawgeti(L, LUA_REGISTRYINDEX, it->func);
        lua_pushinteger(L, addr);
        int err = lua_pcall(L, 1, 0, 0);
        if (err != LUA_OK) {
            const char *msg = lua_tostring(L, -1);
            if (msg) {
                fprintf(
                    stderr,
                    "Error executing trap %p: %s\n",
                    (void *)it,
                    msg
                );
            } else {
                fprintf(
                    stderr,
                    "Error executing trap %p; no error message\n",
                    (void *)it
                );
            }
        }
    }
}
