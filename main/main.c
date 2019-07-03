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

#include <SDL.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <hdbg_gui.h>
#include <hdbg_cpu.h>
#include <hdbg_mem.h>
#include <hdbg_pad.h>
#include <hdbg_trap.h>

#include "../core/r3000a.h"

lua_State *L;

static void cleanup_lua(void)
{
    lua_close(L);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int err = SDL_Init(
        SDL_INIT_AUDIO |
        SDL_INIT_VIDEO |
        SDL_INIT_JOYSTICK |
        SDL_INIT_GAMECONTROLLER
    );
    if (err < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    atexit(SDL_Quit);

    L = luaL_newstate();
    if (!L) {
        fprintf(stderr, "luaL_newstate failed.\n");
        return EXIT_FAILURE;
    }
    atexit(cleanup_lua);

    luaL_openlibs(L);

    err = luaL_loadfile(L, "script.lua");
    if (err != LUA_OK) {
        const char *msg = lua_tostring(L, -1);
        if (msg) {
            fprintf(stderr, "Error loading script.lua: %s\n", msg);
        } else {
            fprintf(stderr, "Error loading script.lua; no error message\n");
        }
        return EXIT_FAILURE;
    }

    gui_init();
    atexit(gui_quit);

    cpu_init();
    atexit(cpu_quit);

    mem_init();
    atexit(mem_quit);

    pad_init();
    atexit(pad_quit);

    trap_init();
    atexit(trap_quit);

    strcpy(Config.Spu, "libpcsx-hdbg-spu.so");
    strcpy(Config.Gpu, "libpcsx-hdbg-gpu.so");
    strcpy(Config.PluginsDir, ".");
    strcpy(Config.Bios, "HLE");
    strcpy(Config.Mcd1, "memcard1.dat");
    strcpy(Config.Mcd2, "memcard2.dat");

    err = EmuInit();
    if (err == -1) {
        fprintf(stderr, "Failed to initialize emulator.\n");
        return EXIT_FAILURE;
    }
    atexit(EmuShutdown);

    SetIsoFile("game.bin");

    err = LoadPlugins();
    if (err == -1) {
        fprintf(stderr, "Failed to load plugins.\n");
        return EXIT_FAILURE;
    }
    atexit(ReleasePlugins);

    err = CDR_open();
    if (err < 0) {
        fprintf(stderr, "Failed to initialize CDR.\n");
        return EXIT_FAILURE;
    }
    atexit((void (*)(void))CDR_close);

    err = SPU_open();
    if (err < 0) {
        fprintf(stderr, "Failed to initialize SPU.\n");
        return EXIT_FAILURE;
    }
    atexit((void (*)(void))SPU_close);

    err = GPU_open(gui_finishframe, "PCSX-HDBG", NULL);
    if (err < 0) {
        fprintf(stderr, "Failed to initialize GPU.\n");
        return EXIT_FAILURE;
    }
    atexit((void (*)(void))GPU_close);

    EmuReset();

    LoadMcds(Config.Mcd1, Config.Mcd2);

    CheckCdrom();
    LoadCdrom();

    err = lua_pcall(L, 0, 0, 0);
    if (err != LUA_OK) {
        const char *msg = lua_tostring(L, -1);
        if (msg) {
            fprintf(stderr, "Error executing script.lua: %s\n", msg);
        } else {
            fprintf(stderr, "Error executing script.lua; no error message\n");
        }
        return EXIT_FAILURE;
    }

    psxCpu->Execute();
}
