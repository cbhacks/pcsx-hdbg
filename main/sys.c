//
// PCSX-HDBG - PCSX-based hack debugger
// Copyright (C) 2019  "chekwob" <chek@wobbyworks.com>
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

void SysUpdate()
{
    PAD1_keypressed();
    PAD2_keypressed();

    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            exit(EXIT_SUCCESS);
        }
    }
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
