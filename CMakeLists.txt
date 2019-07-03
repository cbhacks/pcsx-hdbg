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
find_package (OpenGL REQUIRED)

add_library (pcsx-hdbg-core STATIC
    core/cdriso.c
    core/cdrom.c
    core/cheat.c
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
    core/spu.c
)
target_include_directories (pcsx-hdbg-core
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${ZLIB_INCLUDE_DIRS}"
    PRIVATE "${SDL2_INCLUDE_DIRS}"
)
target_link_libraries (pcsx-hdbg-core
    PUBLIC "${ZLIB_LIBRARIES}"
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
    spu/xa.c
)
target_include_directories (pcsx-hdbg-spu
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${SDL2_INCLUDE_DIRS}"
)
target_link_libraries (pcsx-hdbg-spu
    PRIVATE "${SDL2_LIBRARIES}"
)

add_library (pcsx-hdbg-gpu MODULE
    gpu/cfg.c
    gpu/draw.c
    gpu/fps.c
    gpu/gpu.c
    gpu/gte_accuracy.c
    gpu/key.c
    gpu/menu.c
    gpu/prim.c
    gpu/soft.c
    gpu/texture.c
)
target_include_directories (pcsx-hdbg-gpu
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${SDL2_INCLUDE_DIRS}"
    PRIVATE "${OPENGL_INCLUDE_DIRS}"
)
target_link_libraries (pcsx-hdbg-gpu
    PRIVATE "${SDL2_LIBRARIES}"
    PRIVATE "${OPENGL_LIBRARIES}"
)

add_library (lua STATIC
    lua/src/lapi.c
    lua/src/lauxlib.c
    lua/src/lbaselib.c
    lua/src/lbitlib.c
    lua/src/lcode.c
    lua/src/lcorolib.c
    lua/src/lctype.c
    lua/src/ldblib.c
    lua/src/ldebug.c
    lua/src/ldo.c
    lua/src/ldump.c
    lua/src/lfunc.c
    lua/src/lgc.c
    lua/src/linit.c
    lua/src/liolib.c
    lua/src/llex.c
    lua/src/lmathlib.c
    lua/src/lmem.c
    lua/src/loadlib.c
    lua/src/lobject.c
    lua/src/lopcodes.c
    lua/src/loslib.c
    lua/src/lparser.c
    lua/src/lstate.c
    lua/src/lstring.c
    lua/src/lstrlib.c
    lua/src/ltable.c
    lua/src/ltablib.c
    lua/src/ltm.c
    #lua/src/lua.c
    #lua/src/luac.c
    lua/src/lundump.c
    lua/src/lutf8lib.c
    lua/src/lvm.c
    lua/src/lzio.c
)
target_link_libraries (lua
    PRIVATE "m"
)

add_executable (pcsx-hdbg
    main/main.c
    main/sys.c
    main/gui.c
    main/cpu.c
    main/mem.c
    main/pad.c
    main/trap.c
)
target_include_directories (pcsx-hdbg
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lua/src"
    PRIVATE "${SDL2_INCLUDE_DIRS}"
)
target_compile_options (pcsx-hdbg
    PRIVATE "-Wall"
    PRIVATE "-Wextra"
)
target_link_libraries (pcsx-hdbg
    PRIVATE pcsx-hdbg-core
    PRIVATE lua
    PRIVATE "${SDL2_LIBRARIES}"
)
add_dependencies (pcsx-hdbg
    pcsx-hdbg-gpu
    pcsx-hdbg-spu
)

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/script.lua"
    "${CMAKE_CURRENT_BINARY_DIR}/script.lua"
    COPYONLY
)
