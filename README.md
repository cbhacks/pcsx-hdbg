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
 * Nuklear
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
By default, the game is presented with a single analog controller. The
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

By default, the first available joystick or gamepad input device is also
used to provide input for the analog controller. This should be an SDL2
Game Controller-compatible controller (such as an XInput-compatible one
on Windows). The selected controller may be changed by setting the
`joyid` option in the config file, or disabled completely by setting it
to `nil`.


## Scripting ##
Scripting is available using Lua 5.3. After emulator initialization but
before the CPU starts executing instructions, the `script.lua` file is
executed.

See `docs/scripting.md` for details on the available API's and changes
from the default Lua environment.

The script filename is configurable under the config key `scriptfile`.
If set to nil (or explicitly unset), the script is read from standard
input.


## Configuration ##
Configuration options may be set in `config.lua` or in command line
arguments as key-value pairs. For example:

```sh
pcsx-hdbg keymap.Y=select keymap.U=start scriptfile=/path/to/other/file.lua
```

Other options available:

| Option | Values  | Default |
|--------|---------|---------|
|gamefile|         |game.bin |
|region  |ntsc, pal|ntsc     |


## BIOS ##
The emulator includes an HLE (High Level Emulation) BIOS, which emulates
some functionality of the console's BIOS ROM. This is compatible with some
games (Crash 2), but not others (Crash 3: Warped, CTR). For these, you may
place an appropriate 512K BIOS ROM file into the application directory
under the name `bios.dat`. If this file is not found, the HLE BIOS will be
enabled in its place.
