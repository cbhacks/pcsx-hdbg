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

#include <hdbg_ram.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "lua.h"

#define RAM_ALIGNMASK  0xFFFFFF
#define RAM_SIZE       0x220000

static unsigned char ram_alloc[RAM_ALIGNMASK + 1 + RAM_SIZE];

void *ram_base;

extern lua_State *L;

void ram_init(void)
{
    ram_base = (void *)(((uintptr_t)ram_alloc + RAM_ALIGNMASK) & ~RAM_ALIGNMASK);
    printf("Base RAM address: %p\n", ram_base);

    lua_pushinteger(L, (uintptr_t)ram_base);
    lua_setglobal(L, "ram_base");
}

void ram_quit(void)
{
}
