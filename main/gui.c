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
#include <GL/gl.h>

SDL_Window *gui_window;
SDL_GLContext gui_glctx;
SDL_GLContext gui_gpuglctx;

#define GUI_WIDTH 640
#define GUI_HEIGHT 480

void gui_init(void)
{
    gui_window = SDL_CreateWindow(
        "PCSX-HDBG",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        GUI_WIDTH,
        GUI_HEIGHT,
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

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glOrtho(0.0f, GUI_WIDTH, GUI_HEIGHT, 0.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);

    gui_gpuglctx = SDL_GL_CreateContext(gui_window);
    if (!gui_gpuglctx) {
        fprintf(stderr, "SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        SDL_GL_DeleteContext(gui_glctx);
        SDL_DestroyWindow(gui_window);
        exit(EXIT_FAILURE);
    }
}

void gui_quit(void)
{
    SDL_GL_DeleteContext(gui_gpuglctx);
    SDL_GL_DeleteContext(gui_glctx);
    SDL_DestroyWindow(gui_window);
}

_Bool gui_isopen = 0;

void gui_setopen(_Bool open)
{
    if (gui_isopen == open)
        return;

    if (open) {
        pad_clearkeys();
    }

    gui_isopen = open;
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
            // Toggle GUI open status when pressing escape.
            if (ev.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                if (ev.type == SDL_KEYDOWN) {
                    gui_setopen(!gui_isopen);
                }
                break;
            }

            if (gui_isopen) {
                // TODO
            } else {
                pad_handlekey(ev.key.keysym.scancode, ev.type == SDL_KEYDOWN);
            }
            break;
        }
    }

    extern void update_lua(void);
    update_lua();
}

static void gui_draw(void)
{
    // TODO
}

static void gui_drawbackdrop(void)
{
    glPushAttrib(GL_CURRENT_BIT);
    glEnable(GL_BLEND);

    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
    glVertex2f(0.0f,      0.0f);
    glVertex2f(GUI_WIDTH, 0.0f);
    glColor4f(0.5f, 0.5f, 0.0f, 0.5f);
    glVertex2f(GUI_WIDTH, GUI_HEIGHT);
    glColor4f(0.0f, 0.5f, 0.5f, 0.5f);
    glVertex2f(0.0f,      GUI_HEIGHT);
    glEnd();

    glDisable(GL_BLEND);
    glPopAttrib();
}

void gui_finishframe(void)
{
    if (gui_isopen) {
        glFinish();
        SDL_GL_MakeCurrent(gui_window, gui_glctx);

        gui_drawbackdrop();
        gui_draw();

        glFinish();
        SDL_GL_MakeCurrent(gui_window, gui_gpuglctx);
    }

    SDL_GL_SwapWindow(gui_window);
}
