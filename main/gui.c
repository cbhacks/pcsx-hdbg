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

#define NK_IMPLEMENTATION
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include "nuklear.h"
#pragma GCC diagnostic pop

SDL_Window *gui_window;
SDL_GLContext gui_glctx;
SDL_GLContext gui_gpuglctx;
struct nk_context gui_nkctx;

#define GUI_FONT_SIZE 16.0f

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

    static struct nk_font *font;
    static struct nk_font_atlas font_atlas;
    int font_width;
    int font_height;
    const void *font_pixels;
    nk_font_atlas_init_default(&font_atlas);
    nk_font_atlas_begin(&font_atlas);
    font = nk_font_atlas_add_default(&font_atlas, GUI_FONT_SIZE, NULL);
    font_pixels = nk_font_atlas_bake(
        &font_atlas,
        &font_width,
        &font_height,
        NK_FONT_ATLAS_RGBA32
    );
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        font_width,
        font_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        font_pixels
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    nk_font_atlas_end(&font_atlas, nk_handle_id(0xDEAD), 0);

    nk_init_default(&gui_nkctx, &font->handle);

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
    const float window_spacing = 80.0f;
    const struct nk_rect window_rect = {
        window_spacing,
        window_spacing,
        GUI_WIDTH - 2 * window_spacing,
        GUI_HEIGHT - 2 * window_spacing
    };
    if (nk_begin(&gui_nkctx, "PCSX-HDBG (" __DATE__ ")", window_rect, NK_WINDOW_TITLE)) {
        nk_layout_row_dynamic(&gui_nkctx, 30, 1);
        nk_label(&gui_nkctx, "Sample Text", NK_TEXT_LEFT);
    }
    nk_end(&gui_nkctx);

    glPushAttrib(GL_CURRENT_BIT | GL_SCISSOR_BIT);
    glEnable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);

    struct vtx {
        float pos[2];
        float uvs[2];
        unsigned char col[4];
    };

    struct nk_draw_vertex_layout_element layout[] = {
        { NK_VERTEX_POSITION, NK_FORMAT_FLOAT, offsetof(struct vtx, pos) },
        { NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, offsetof(struct vtx, uvs) },
        { NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, offsetof(struct vtx, col) },
        { NK_VERTEX_LAYOUT_END }
    };

    struct nk_convert_config cfg = {};
    cfg.vertex_layout = layout;
    cfg.vertex_size = sizeof(struct vtx);
    cfg.vertex_alignment = _Alignof(struct vtx);
    cfg.global_alpha = 0.8f;

    struct nk_buffer cmds;
    struct nk_buffer vbuf;
    struct nk_buffer ebuf;
    nk_buffer_init_default(&cmds);
    nk_buffer_init_default(&vbuf);
    nk_buffer_init_default(&ebuf);

    nk_convert(&gui_nkctx, &cmds, &vbuf, &ebuf, &cfg);

    const nk_byte *vertices = nk_buffer_memory_const(&vbuf);
    const nk_ushort *elements = nk_buffer_memory_const(&ebuf);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(2, GL_FLOAT, sizeof(struct vtx), vertices + offsetof(struct vtx, pos));
    glTexCoordPointer(2, GL_FLOAT, sizeof(struct vtx), vertices + offsetof(struct vtx, uvs));
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct vtx), vertices + offsetof(struct vtx, col));

    const struct nk_draw_command *cmd;
    nk_draw_foreach(cmd, &gui_nkctx, &cmds) {
        if (!cmd->elem_count)
            continue;

        // TODO - scissor

        if (cmd->texture.id == 0xDEAD)
            glEnable(GL_TEXTURE_2D);

        glDrawElements(GL_TRIANGLES, cmd->elem_count, GL_UNSIGNED_SHORT, elements);
        elements += cmd->elem_count;

        glDisable(GL_TEXTURE_2D);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    nk_buffer_free(&cmds);
    nk_buffer_free(&vbuf);
    nk_buffer_free(&ebuf);

    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glPopAttrib();

    nk_clear(&gui_nkctx);
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
