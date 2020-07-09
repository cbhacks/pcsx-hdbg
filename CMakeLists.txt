#
# PCSX-HDBG - PCSX-based hack debugger
# Copyright (C) 2019-2020  "chekwob" <chek@wobbyworks.com>
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

cmake_minimum_required (VERSION 3.7)
project (PCSXHDBG LANGUAGES C)

set (BUILD_LEVEL "STANDARD" CACHE STRING
    "Used only for cross-compiling when unable to run target bins on host."
)
set_property (CACHE BUILD_LEVEL PROPERTY STRINGS
    "STANDARD"
    "CROSS_HOST"
    "CROSS_TARGET"
)
set (HOST_EXPORTS_FILE "NOTFOUND" CACHE FILEPATH
    "Used only for cross-compiling when unable to run target bins on host."
)
if (HOST_EXPORTS_FILE STREQUAL "NOTFOUND" AND NOT (BUILD_LEVEL STREQUAL "STANDARD"))
    message (FATAL_ERROR "When cross compiling, you must set HOST_EXPORTS_FILE to an absolute path.")
endif ()

if (BUILD_LEVEL STREQUAL "STANDARD" OR BUILD_LEVEL STREQUAL "CROSS_HOST")
    add_executable (bin2c tools/bin2c.c)
endif ()

if (BUILD_LEVEL STREQUAL "STANDARD" OR BUILD_LEVEL STREQUAL "CROSS_TARGET")
    find_package (ZLIB REQUIRED)
    find_package (SDL2 REQUIRED)
    find_package (OpenGL REQUIRED)

    # Fix an SDL2 bug (error CMP0004).
    # https://bugzilla.libsdl.org/show_bug.cgi?id=3295
    string (STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)

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
        PUBLIC "${ZLIB_INCLUDE_DIR}"
        PUBLIC "${SDL2_INCLUDE_DIRS}"
    )
    target_compile_options (pcsx-hdbg-core
        PRIVATE "-Wno-unused-result"
    )
    target_link_libraries (pcsx-hdbg-core
        PRIVATE "${ZLIB_LIBRARIES}"
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
        PUBLIC "${SDL2_INCLUDE_DIRS}"
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
        PUBLIC "${SDL2_INCLUDE_DIRS}"
        PUBLIC "${OPENGL_INCLUDE_DIRS}"
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
        main/ram.c
        main/mem.c
        main/pad.c
        main/cdr.c
        main/trap.c
        "${CMAKE_CURRENT_BINARY_DIR}/init.lua.c"
    )
    target_include_directories (pcsx-hdbg
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lua/src"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/nuklear"
        PRIVATE "${SDL2_INCLUDE_DIRS}"
        PRIVATE "${ZLIB_INCLUDE_DIR}"
        PRIVATE "${OPENGL_INCLUDE_DIRS}"
    )
    target_compile_options (pcsx-hdbg
        PRIVATE "-Wall"
        PRIVATE "-Wextra"
    )
    target_link_libraries (pcsx-hdbg
        PRIVATE pcsx-hdbg-core
        PRIVATE lua
        PRIVATE "${SDL2_LIBRARIES}"
        PRIVATE "${OPENGL_LIBRARIES}"
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

    add_custom_command (
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/init.lua.c"
        COMMAND bin2c
        ARGS
            "${CMAKE_CURRENT_SOURCE_DIR}/init.lua"
            "${CMAKE_CURRENT_BINARY_DIR}/init.lua.c"
            init_lua
        DEPENDS init.lua
        VERBATIM
    )
endif ()

if (BUILD_LEVEL STREQUAL "CROSS_HOST")
    export (
        TARGETS
            bin2c
        FILE "${HOST_EXPORTS_FILE}"
    )
elseif (BUILD_LEVEL STREQUAL "CROSS_TARGET")
    include ("${HOST_EXPORTS_FILE}")
endif ()
