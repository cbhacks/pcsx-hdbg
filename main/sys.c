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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <SDL.h>
#include "lua.h"

#include "../core/r3000a.h"

extern lua_State *L;

void sys_init(void)
{
    lua_getglobal(L, "config");

    lua_getfield(L, -1, "gamefile");
    int t_type = lua_type(L, -1);
    if (t_type != LUA_TSTRING) {
        fprintf(
            stderr,
            "Error in config: Bad gamefile (was %s, expected string)\n",
            lua_typename(L, t_type)
        );
        exit(EXIT_FAILURE);
    }

    const char *str = lua_tolstring(L, -1, NULL);
    SetIsoFile(str);
    lua_pop(L, 1);

    lua_getfield(L, -1, "region");
    t_type = lua_type(L, -1);
    if (t_type != LUA_TSTRING) {
        fprintf(
            stderr,
            "Error in config: Bad region (was %s, expected string)\n",
            lua_typename(L, t_type)
        );
        exit(EXIT_FAILURE);
    }

    str = lua_tolstring(L, -1, NULL);
    if (strcasecmp(str, "ntsc") == 0) {
        printf("Selected region: NTSC\n");
        Config.PsxType = PSX_TYPE_NTSC;
        lua_pushboolean(L, 1);
        lua_setglobal(L, "ntsc");
        lua_pushboolean(L, 0);
        lua_setglobal(L, "pal");
    } else if (strcasecmp(str, "pal") == 0) {
        printf("Selected region: PAL\n");
        Config.PsxType = PSX_TYPE_PAL;
        lua_pushboolean(L, 0);
        lua_setglobal(L, "ntsc");
        lua_pushboolean(L, 1);
        lua_setglobal(L, "pal");
    } else {
        fprintf(
            stderr,
            "Unrecognized region '%s'; defaulting to NTSC\n",
            str
        );
        Config.PsxType = PSX_TYPE_NTSC;
        lua_pushboolean(L, 1);
        lua_setglobal(L, "ntsc");
        lua_pushboolean(L, 0);
        lua_setglobal(L, "pal");
    }
    lua_pop(L, 1);

    lua_pop(L, 1);
}

void sys_quit(void)
{
}

#include "../core/system.h"
#include "../core/plugins.h"
#include "../core/psxcommon.h"

void SysReset()
{
    EmuReset();
}

void SysPrintf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

void SysMessage(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

static const char *sys_lib_error;

void *SysLoadLibrary(const char *lib)
{
    void *result = SDL_LoadObject(lib);
    sys_lib_error = result ? NULL : SDL_GetError();
    return result;
}

void *SysLoadSym(void *lib, const char *sym)
{
    void *result = SDL_LoadFunction(lib, sym);
    sys_lib_error = result ? NULL : SDL_GetError();
    return result;
}

const char *SysLibError()
{
    return sys_lib_error;
}

void SysCloseLibrary(void *lib)
{
    SDL_UnloadObject(lib);
}

void SysRunGui()
{
    // TODO
}

void SysClose()
{
    // TODO
}

void ClosePlugins()
{
    abort();
}
