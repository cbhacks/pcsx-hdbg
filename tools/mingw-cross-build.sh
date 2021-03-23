#
# PCSX-HDBG - PCSX-based hack debugger
# Copyright (C) 2019-2021  "chekwob" <chek@wobbyworks.com>
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

set -e

if [ -z "${ARCH}" ]; then
    echo 'You must set $ARCH when running this script!'
    echo ''
    echo 'Normally acceptable values:'
    echo '  i686'
    echo '  x86_64'
    exit 1
fi

HOST="${ARCH}-w64-mingw32"

echo "Building for host: ${HOST}"

SOURCE_DIR="$(cd "$(dirname ${BASH_SOURCE})/.." && pwd)"
TEMP_DIR="$(mktemp -d)"
echo "Operating in temporary directory: ${TEMP_DIR}"

HOST_BUILD_DIR="${TEMP_DIR}/build-host"
mkdir "${HOST_BUILD_DIR}"

TARGET_BUILD_DIR="${TEMP_DIR}/build-target"
mkdir "${TARGET_BUILD_DIR}"

PREFIX_DIR="${TEMP_DIR}/prefix"
mkdir "${PREFIX_DIR}"

PKG_DIR="${TEMP_DIR}/pkg"
mkdir "${PKG_DIR}"

(
    echo "Downloading zlib source..."
    cd "${TEMP_DIR}"
    wget http://zlib.net/zlib-1.2.11.tar.xz

    echo "Verifying downloaded file with SHA512..."
    echo "b7f50ada138c7f93eb7eb1631efccd1d9f03a5e77b6c13c8b757017b2d462e19d2d3e01c50fad60a4ae1bc86d431f6f94c72c11ff410c25121e571953017cb67  zlib-1.2.11.tar.xz" | sha512sum -c --strict

    echo "Extracting zlib source archive..."
    tar xJf zlib-1.2.11.tar.xz

    echo "Building zlib..."
    cd zlib-1.2.11
    sed "s/PREFIX =/PREFIX = ${HOST}-/" -i win32/Makefile.gcc
    export prefix=""
    export DESTDIR="${PREFIX_DIR}"
    export BINARY_PATH=/bin
    export LIBRARY_PATH=/lib
    export INCLUDE_PATH=/include
    make -f win32/Makefile.gcc
    make -f win32/Makefile.gcc install
)

(
    echo "Downloading SDL2 development libraries for MinGW..."
    cd "${TEMP_DIR}"
    wget https://libsdl.org/release/SDL2-devel-2.0.9-mingw.tar.gz

    echo "Verifying downloaded file with SHA512..."
    echo "ac2422fbd96aac0584755f6e3de7832f3f6b7cff0a077e0ff500419540d61aa5e26c7280c16ff65e52a69f4ecda9b6ee8282b1b8b735cba6b141b47a801f2191  SDL2-devel-2.0.9-mingw.tar.gz" | sha512sum -c --strict

    echo "Extracting SDL2 development library archive..."
    tar xzf SDL2-devel-2.0.9-mingw.tar.gz

    echo "Setting up SDL2 development library files..."
    cp -r "SDL2-2.0.9/${HOST}/." "${PREFIX_DIR}/."
    sed "s|/opt/local/${HOST}|${PREFIX_DIR}|" -i "${PREFIX_DIR}/lib/cmake/SDL2/sdl2-config.cmake"
)

(
    echo "Building PCSX-HDBG build tools for host..."
    cd "${HOST_BUILD_DIR}"
    cmake "${SOURCE_DIR}" \
        -DBUILD_LEVEL=CROSS_HOST \
        -DHOST_EXPORTS_FILE="${TEMP_DIR}/host-exports.cmake"
    make
)

(
    echo "Building PCSX-HDBG for target..."
    cd "${TARGET_BUILD_DIR}"
    cmake "${SOURCE_DIR}" \
        -DBUILD_LEVEL=CROSS_TARGET \
        -DHOST_EXPORTS_FILE="${TEMP_DIR}/host-exports.cmake" \
        -DCMAKE_SYSTEM_NAME=Windows \
        -DCMAKE_C_COMPILER="${HOST}-gcc" \
        -DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -mconsole" \
        -DCMAKE_SHARED_LINKER_FLAGS="-static-libgcc" \
        -DCMAKE_MODULE_LINKER_FLAGS="-static-libgcc" \
        -DCMAKE_FIND_ROOT_PATH="/usr/${HOST};${PREFIX_DIR}" \
        -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
        -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
        -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY
    make
)

(
    pkg() {
        cp -r "$1" "${PKG_DIR}/$(basename "$1")"
    }

    echo "Packaging software..."
    pkg "${SOURCE_DIR}/README.md"
    pkg "${SOURCE_DIR}/COPYING"
    pkg "${SOURCE_DIR}/AUTHORS"
    pkg "${SOURCE_DIR}/docs"
    pkg "${TARGET_BUILD_DIR}/pcsx-hdbg.exe"
    pkg "${TARGET_BUILD_DIR}/libpcsx-hdbg-spu.dll"
    pkg "${TARGET_BUILD_DIR}/libpcsx-hdbg-gpu.dll"
    pkg "${TARGET_BUILD_DIR}/script.lua"
    pkg "${PREFIX_DIR}/bin/SDL2.dll"
    rm -f "pcsx-hdbg.zip"
    7z a "pcsx-hdbg.zip" "${PKG_DIR}/"'*'
)
