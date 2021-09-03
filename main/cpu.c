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
#include <hdbg_cpu.h>

#include "lua.h"
#include "lauxlib.h"

#include "../core/r3000a.h"

static int scr_getreg(lua_State *L)
{
    int reg = luaL_checkinteger(L, 1) - 1;
    if (reg < 0 || reg >= 34 + 32 + 32 + 32 + 1) {
        return luaL_error(L, "register index out of bounds");
    }
    lua_pushinteger(L, (uint32_t)psxRegs.GPR.r[reg]);
    return 1;
}

static int scr_setreg(lua_State *L)
{
    int reg = luaL_checkinteger(L, 1) - 1;
    uint32_t value = luaL_checkinteger(L, 2);
    if (reg < 0 || reg >= 34 + 32 + 32 + 32 + 1) {
        return luaL_error(L, "register index out of bounds");
    }
    if (reg == 0) {
        return luaL_error(L, "attempted to set register r0");
    }
    psxRegs.GPR.r[reg] = value;
    return 0;
}

extern lua_State *L;

#define DEFINE_LUA_FUNCTION(name) \
    lua_pushcfunction(L, scr_##name); \
    lua_setglobal(L, #name);

void cpu_init(void)
{
    DEFINE_LUA_FUNCTION(getreg);
    DEFINE_LUA_FUNCTION(setreg);
}

void cpu_quit(void)
{
}
