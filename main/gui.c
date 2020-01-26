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

#include <hdbg_gui.h>

#include <stdio.h>
#include <stdlib.h>

#include <hdbg_pad.h>

#include <SDL.h>

SDL_Window *gui_window;
SDL_GLContext gui_glctx;

void gui_init(void)
{
    gui_window = SDL_CreateWindow(
        "PCSX-HDBG",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640,
        480,
        SDL_WINDOW_OPENGL
    );
    if (!gui_window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    gui_glctx = SDL_GL_CreateContext(gui_window);
    if (!gui_glctx) {
        fprintf(stderr, "SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(gui_window);
        exit(EXIT_FAILURE);
    }
}

void gui_quit(void)
{
    SDL_GL_DeleteContext(gui_glctx);
    SDL_DestroyWindow(gui_window);
}

void gui_update(void)
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
        case SDL_QUIT:
            exit(EXIT_SUCCESS);
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            pad_handlekey(ev.key.keysym.scancode, ev.type == SDL_KEYDOWN);
            break;
        }
    }

    extern void update_lua(void);
    update_lua();
}

void gui_finishframe(void)
{
    SDL_GL_SwapWindow(gui_window);
}
