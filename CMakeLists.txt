#
# PCSX-HDBG - PCSX-based hack debugger
# Copyright (C) 2019  "chekwob" <chek@wobbyworks.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required (VERSION 3.8)
project (PCSXHDBG)

find_package (ZLIB REQUIRED)
find_package (SDL2 REQUIRED)
find_package (X11 REQUIRED)
find_package (OpenGL REQUIRED)

find_package (PkgConfig REQUIRED)
pkg_search_module (GTK3 REQUIRED gtk+-3.0)

add_library (pcsx-hdbg-core STATIC
    core/cdriso.c
    core/cdrom.c
    core/cheat.c
    core/debug.c
    core/decode_xa.c
    core/disr3000a.c
    core/gpu.c
    core/gte.c
    core/mdec.c
    core/misc.c
    core/plugins.c
    core/ppf.c
    core/psxbios.c
    core/psxcommon.c
    core/psxcounters.c
    core/psxdma.c
    core/psxhle.c
    core/psxhw.c
    core/psxinterpreter.c
    core/psxmem.c
    core/r3000a.c
    core/sio.c
    core/socket.c
    core/spu.c
)
target_include_directories (pcsx-hdbg-core
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${ZLIB_INCLUDE_DIRS}"
)
target_link_libraries (pcsx-hdbg-core
    PUBLIC "${ZLIB_LIBRARIES}"
    PUBLIC "rt"
)

add_library (pcsx-hdbg-spu MODULE
    spu/adsr.c
    spu/cfg.c
    spu/dma.c
    spu/externals.c
    spu/freeze.c
    spu/registers.c
    spu/reverb.c
    spu/sdl.c
    spu/spu.c
    spu/spucfg-0.1df/main.c
    spu/xa.c
)
target_include_directories (pcsx-hdbg-spu
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${SDL2_INCLUDE_DIRS}"
    PRIVATE "${GTK3_INCLUDE_DIRS}"
)
target_compile_options (pcsx-hdbg-spu
    PRIVATE "${GTK3_CFLAGS_OTHER}"
)
target_link_libraries (pcsx-hdbg-spu
    PRIVATE "${SDL2_LIBRARIES}"
    PRIVATE "${GTK3_LIBRARIES}"
)

add_library (pcsx-hdbg-gpu MODULE
    gpu/cfg.c
    gpu/draw.c
    gpu/fps.c
    gpu/gpu.c
    gpu/gpucfg/main.c
    gpu/gte_accuracy.c
    gpu/key.c
    gpu/menu.c
    gpu/prim.c
    gpu/soft.c
    gpu/texture.c
)
target_include_directories (pcsx-hdbg-gpu
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${X11_INCLUDE_DIRS}"
    PRIVATE "${OPENGL_INCLUDE_DIRS}"
    PRIVATE "${GTK3_INCLUDE_DIRS}"
)
target_compile_options (pcsx-hdbg-gpu
    PRIVATE "${GTK3_CFLAGS_OTHER}"
)
target_link_libraries (pcsx-hdbg-gpu
    PRIVATE "${X11_LIBRARIES}"
    PRIVATE "${OPENGL_LIBRARIES}"
    PRIVATE "${GTK3_LIBRARIES}"
)

add_library (pcsx-hdbg-pad MODULE
    pad/analog.c
    pad/cfg.c
    pad/pad.c
    pad/sdljoy.c
    pad/util.c
    pad/xkb.c
)
target_include_directories (pcsx-hdbg-pad
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${SDL2_INCLUDE_DIRS}"
    PRIVATE "${GTK3_INCLUDE_DIRS}"
)
target_compile_options (pcsx-hdbg-pad
    PRIVATE "${GTK3_CFLAGS_OTHER}"
)
target_link_libraries (pcsx-hdbg-pad
    PRIVATE "${SDL2_LIBRARIES}"
    PRIVATE "${GTK3_LIBRARIES}"
)

add_executable (pcsx-hdbg
    main/main.c
    main/sys.c
)
target_include_directories (pcsx-hdbg
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${SDL2_INCLUDE_DIRS}"
)
target_compile_options (pcsx-hdbg
    PRIVATE "-Wall"
    PRIVATE "-Wextra"
)
target_link_libraries (pcsx-hdbg
    PRIVATE pcsx-hdbg-core
    PRIVATE "${SDL2_LIBRARIES}"
)
add_dependencies (pcsx-hdbg
    pcsx-hdbg-gpu
    pcsx-hdbg-spu
    pcsx-hdbg-pad
)
