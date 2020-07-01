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

#include <SDL.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <hdbg_sys.h>
#include <hdbg_gui.h>
#include <hdbg_cpu.h>
#include <hdbg_ram.h>
#include <hdbg_mem.h>
#include <hdbg_pad.h>
#include <hdbg_trap.h>

#include "../core/r3000a.h"

lua_State *L;
static lua_State *Lupdatethread;
static int updatethread_ref;
static int updatethread_suspended = 0;

static void cleanup_lua(void)
{
    lua_close(L);
}

static _Noreturn void panic_lua(void)
{
    const char *msg = lua_tostring(L, -1);
    if (msg) {
        fprintf(stderr, "Error in internal lua scripts: %s\n", msg);
    } else {
        fprintf(stderr, "Error in internal lua scripts; no error message\n");
    }
    exit(EXIT_FAILURE);
}

#ifdef _WIN32
static int scr_osexit(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    bool success = lua_toboolean(L, 1);

    printf("Program terminated by script (%s).\n", success ? "success" : "failure");
    system("pause");
    exit(success ? EXIT_SUCCESS : EXIT_FAILURE);
}
#endif

void update_lua(void)
{
    if (updatethread_suspended)
        return;
    if (!Lupdatethread)
        return;

    int err = lua_resume(Lupdatethread, NULL, 0);
    if (err == LUA_YIELD) {
        lua_settop(Lupdatethread, 0);
    } else if (err == LUA_OK) {
        lua_settop(Lupdatethread, 0);
        Lupdatethread = NULL;
        luaL_unref(L, LUA_REGISTRYINDEX, updatethread_ref);
    } else {
        lua_xmove(Lupdatethread, L, 1);
        panic_lua();
    }
}

#ifdef main
#undef main
#endif
int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int err = SDL_Init(
        SDL_INIT_AUDIO |
        SDL_INIT_VIDEO
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

#ifdef _WIN32
    lua_getglobal(L, "os");
    lua_pushcfunction(L, scr_osexit);
    lua_setfield(L, -2, "exit");
    lua_pop(L, 1);
#endif

    lua_newtable(L);
    for (int i = 1; i < argc; i++) {
        lua_pushstring(L, argv[i]);
        lua_rawseti(L, -2, i);
    }
    lua_setglobal(L, "argv");

    extern const char init_lua_data[];
    extern const size_t init_lua_size;
    err = luaL_loadbuffer(L, init_lua_data, init_lua_size, "init.lua");
    if (err != LUA_OK) {
        panic_lua();
    }

    Lupdatethread = lua_newthread(L);
    updatethread_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_xmove(L, Lupdatethread, 1);
    update_lua();
    updatethread_suspended = 1;

    sys_init();
    atexit(sys_quit);

    gui_init();
    atexit(gui_quit);

    cpu_init();
    atexit(cpu_quit);

    ram_init();
    atexit(ram_quit);

    mem_init();
    atexit(mem_quit);

    pad_init();
    atexit(pad_quit);

    trap_init();
    atexit(trap_quit);

#ifndef _WIN32
    strcpy(Config.Spu, "./libpcsx-hdbg-spu.so");
    strcpy(Config.Gpu, "./libpcsx-hdbg-gpu.so");
#else
    strcpy(Config.Spu, "libpcsx-hdbg-spu.dll");
    strcpy(Config.Gpu, "libpcsx-hdbg-gpu.dll");
#endif
    strcpy(Config.Bios, "bios.dat");
    strcpy(Config.Mcd1, "memcard1.dat");
    strcpy(Config.Mcd2, "memcard2.dat");

    Config.PsxOut = 1;

    err = EmuInit();
    if (err == -1) {
        fprintf(stderr, "Failed to initialize emulator.\n");
        return EXIT_FAILURE;
    }
    atexit(EmuShutdown);

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

    // Execute BIOS until EXE entry point.
    while (psxRegs.pc < 0x80010000 || psxRegs.pc > 0x801FFFFF) {
        psxCpu->ExecuteBlock();
    }

    updatethread_suspended = 0;
    update_lua();

    psxCpu->Execute();
}
