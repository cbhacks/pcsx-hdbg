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

#include <hdbg_pad.h>

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

static uint16_t pad_buttons = 0xFFFF;
static uint16_t pad_bindings[SDL_NUM_SCANCODES];

void pad_init(void)
{
    for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
        pad_bindings[i] = -1;
    }

    pad_bindings[SDL_SCANCODE_C]     = PAD_BUTTON_SELECT;
    pad_bindings[SDL_SCANCODE_V]     = PAD_BUTTON_START;
    pad_bindings[SDL_SCANCODE_UP]    = PAD_BUTTON_UP;
    pad_bindings[SDL_SCANCODE_RIGHT] = PAD_BUTTON_RIGHT;
    pad_bindings[SDL_SCANCODE_DOWN]  = PAD_BUTTON_DOWN;
    pad_bindings[SDL_SCANCODE_LEFT]  = PAD_BUTTON_LEFT;
    pad_bindings[SDL_SCANCODE_E]     = PAD_BUTTON_L2;
    pad_bindings[SDL_SCANCODE_T]     = PAD_BUTTON_R2;
    pad_bindings[SDL_SCANCODE_W]     = PAD_BUTTON_L1;
    pad_bindings[SDL_SCANCODE_R]     = PAD_BUTTON_R1;
    pad_bindings[SDL_SCANCODE_D]     = PAD_BUTTON_TRIANGLE;
    pad_bindings[SDL_SCANCODE_X]     = PAD_BUTTON_CIRCLE;
    pad_bindings[SDL_SCANCODE_Z]     = PAD_BUTTON_X;
    pad_bindings[SDL_SCANCODE_S]     = PAD_BUTTON_SQUARE;
}

void pad_quit(void)
{
}

uint16_t pad_getbuttons(void)
{
    return pad_buttons;
}

void pad_handlekey(SDL_Scancode scancode, int down)
{
    int button = pad_bindings[scancode];
    if (button == -1)
        return;

    uint16_t buttonmask = 1 << button;

    if (down) {
        pad_buttons &= ~buttonmask;
    } else {
        pad_buttons |= buttonmask;
    }
}
