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

#include <hdbg_mem.h>

#include "lua.h"
#include "lauxlib.h"

#include "../core/psxmem.h"

static int scr_readu8(lua_State *L)
{
    uint32_t addr = luaL_checkinteger(L, 1);
    lua_pushinteger(L, (uint8_t)psxMemRead8(addr));
    return 1;
}

static int scr_reads8(lua_State *L)
{
    uint32_t addr = luaL_checkinteger(L, 1);
    lua_pushinteger(L, (int8_t)psxMemRead8(addr));
    return 1;
}

static int scr_readu16(lua_State *L)
{
    uint32_t addr = luaL_checkinteger(L, 1);
    lua_pushinteger(L, (uint16_t)psxMemRead16(addr));
    return 1;
}

static int scr_reads16(lua_State *L)
{
    uint32_t addr = luaL_checkinteger(L, 1);
    lua_pushinteger(L, (int16_t)psxMemRead16(addr));
    return 1;
}

static int scr_readu32(lua_State *L)
{
    uint32_t addr = luaL_checkinteger(L, 1);
    lua_pushinteger(L, (uint32_t)psxMemRead32(addr));
    return 1;
}

static int scr_reads32(lua_State *L)
{
    uint32_t addr = luaL_checkinteger(L, 1);
    lua_pushinteger(L, (int32_t)psxMemRead32(addr));
    return 1;
}

static int scr_write8(lua_State *L)
{
    uint32_t addr = luaL_checkinteger(L, 1);
    uint8_t value = luaL_checkinteger(L, 2);
    psxMemWrite8(addr, value);
    return 0;
}

static int scr_write16(lua_State *L)
{
    uint32_t addr = luaL_checkinteger(L, 1);
    uint16_t value = luaL_checkinteger(L, 2);
    psxMemWrite16(addr, value);
    return 0;
}

static int scr_write32(lua_State *L)
{
    uint32_t addr = luaL_checkinteger(L, 1);
    uint32_t value = luaL_checkinteger(L, 2);
    psxMemWrite32(addr, value);
    return 0;
}

extern lua_State *L;

#define DEFINE_LUA_FUNCTION(name) \
    lua_pushcfunction(L, scr_##name); \
    lua_setglobal(L, #name);

void mem_init(void)
{
    DEFINE_LUA_FUNCTION(readu8);
    DEFINE_LUA_FUNCTION(reads8);
    DEFINE_LUA_FUNCTION(readu16);
    DEFINE_LUA_FUNCTION(reads16);
    DEFINE_LUA_FUNCTION(readu32);
    DEFINE_LUA_FUNCTION(reads32);
    DEFINE_LUA_FUNCTION(write8);
    DEFINE_LUA_FUNCTION(write16);
    DEFINE_LUA_FUNCTION(write32);
}

void mem_quit(void)
{
}
