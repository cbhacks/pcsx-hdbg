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

#include "../core/r3000a.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    strcpy(Config.Spu, "libpcsx-hdbg-spu.so");
    strcpy(Config.Gpu, "libpcsx-hdbg-gpu.so");
    strcpy(Config.Pad1, "libpcsx-hdbg-pad.so");
    strcpy(Config.Pad2, "libpcsx-hdbg-pad.so");
    strcpy(Config.PluginsDir, ".");
    strcpy(Config.Bios, "HLE");

    int err = EmuInit();
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

    unsigned long display;

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

    err = GPU_open(&display, "PCSX-HDBG", NULL);
    if (err < 0) {
        fprintf(stderr, "Failed to initialize GPU.\n");
        return EXIT_FAILURE;
    }
    atexit((void (*)(void))GPU_close);

    err = PAD1_open(&display);
    if (err < 0) {
        fprintf(stderr, "Failed to initialize PAD1.\n");
        return EXIT_FAILURE;
    }
    atexit((void (*)(void))PAD1_close);

    err = PAD2_open(&display);
    if (err < 0) {
        fprintf(stderr, "Failed to initialize PAD2.\n");
        return EXIT_FAILURE;
    }
    atexit((void (*)(void))PAD2_close);

    EmuReset();

    psxCpu->Execute();
}
