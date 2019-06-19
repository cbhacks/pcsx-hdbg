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

#include <dlfcn.h>

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

void *SysLoadLibrary(const char *lib)
{
    return dlopen(lib, RTLD_NOW);
}

void *SysLoadSym(void *lib, const char *sym)
{
    return dlsym(lib, sym);
}

const char *SysLibError()
{
    return dlerror();
}

void SysCloseLibrary(void *lib)
{
    dlclose(lib);
}

void SysUpdate()
{
    PAD1_keypressed();
    PAD2_keypressed();
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
