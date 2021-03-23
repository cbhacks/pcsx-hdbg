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

#ifndef H__HDBG_GUI__H
#define H__HDBG_GUI__H

#include <SDL.h>

extern float gui_scale;

#define GUI_WIDTH ((int)(1024 * gui_scale))
#define GUI_HEIGHT ((int)(768 * gui_scale))

extern SDL_Window *gui_window;
extern SDL_GLContext gui_glctx;
extern SDL_GLContext gui_gpuglctx;

void gui_init(void);
void gui_quit(void);

extern _Bool gui_isopen;

void gui_setopen(_Bool open);

void gui_update(void);

void gui_finishframe(void);

#endif
