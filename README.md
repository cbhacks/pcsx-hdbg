# PCSX-HDBG #
PCSX-HDBG is a fork of PCSX-r providing enhanced debugging and hacking
capability.


## License ##
This software is made available under the terms of the
*GNU General Public License*, version 3 or later.

See the AUTHORS and COPYING files for more details.

Some other software may be included with source and binary distributions
of this software:

 * Lua 5.3
 * SDL 2.0 _(Windows binary releases only)_
 * zlib _(Windows binary releases only)_

In Windows binary releases, some of these libraries are statically
linked into the main executable.


## Building ##
This software is designed to be built using CMake, version 3.7 or later.

```
[pcsx-hdbg]$ mkdir build
[pcsx-hdbg]$ cd build
[pcsx-hdbg/build]$ cmake ..
[pcsx-hdbg/build]$ make
[pcsx-hdbg/build]$ ./pcsx-hdbg
```

Binary releases for Windows are built under a MinGW cross-compilation
environment, using a script in the `tools` directory.


## Controls ##
By default, the game is presented with a single digital controller. The
controller buttons are bound to the following keys by default:

| Key | Button |
|-----|--------|
|C    |select  |
|     |l3      |
|     |r3      |
|V    |start   |
|Up   |up      |
|Right|right   |
|Down |down    |
|Left |left    |
|E    |l2      |
|T    |r2      |
|W    |l1      |
|R    |r1      |
|D    |triangle|
|X    |circle  |
|Z    |x       |
|S    |square  |

These controls may be changed in the `config.lua` file, like so:

```lua
keymap["Left Shift"] = "circle"
keymap["Space"] = "x"
```

Key names are taken from <https://wiki.libsdl.org/SDL_Keycode>, while
button names are from the table above. Both key and button names are
case-sensitive.


## Scripting ##
Scripting is available using Lua 5.3. After emulator initialization but
before the CPU starts executing instructions, the `script.lua` file is
executed.

See `docs/scripting.md` for details on the available API's and changes
from the default Lua environment.
