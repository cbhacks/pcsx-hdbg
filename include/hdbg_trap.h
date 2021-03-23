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

#ifndef H__HDBG_TRAP__H
#define H__HDBG_TRAP__H

#include <stdint.h>

#define TRAP_HASH(addr) ((addr) & 0xFFFF)

#define CONSIDER_TRAP(addr) (trap_chains[TRAP_HASH(addr)] != NULL)

extern struct trap *trap_chains[];

void trap_init(void);
void trap_quit(void);

void trap_raise(uint32_t addr);

#endif
