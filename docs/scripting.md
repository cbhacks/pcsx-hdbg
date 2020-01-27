# Scripting #
**TBD**

Except as specified in this document, most standard Lua 5.3 API's should
be available for use, even including `package` and `os`.


## Undefined Globals ##
If a global variable is read before it has been defined, an error is
raised. This has been added to prevent errors due to typos, for example:

Vanilla Lua:
```lua
local hello = "Hello!"
print(hello)            -- OK, prints "Hello!"
print(hellp)            -- OK, prints nil
```

PCSX-HDBG Lua:
```lua
local hello = "Hello!"
print(hello)            -- OK, prints "Hello!"
print(hellp)            -- Error, undefined global "hellp"
```

To explicitly define a global variable, initialize it with any value.

```lua
local my_local
my_global = nil

do_stuff(my_local)      -- OK, "my_local" is local
do_stuff(my_global)     -- OK, "my_global" is global
```


## Utility ##
* `printf(...)`

This function takes the same parameters as Lua's `string.format`, but
the result is printed rather than returned.


## Memory Access ##
* `reads8(addr) => value`
* `reads16(addr) => value`
* `reads32(addr) => value`
* `readu8(addr) => value`
* `readu16(addr) => value`
* `readu32(addr) => value`

Each of these functions takes an address (word) and returns an integer,
sign-extended for `s` functions, zero-extended for `u`-functions, which
is the result of reading a byte, halfword, or word (for `8`, `16`, and
`32`, respectively) from emulator memory. The memory address given may
correspond to a hardware I/O register or other unmapped memory, not
necessarily only the basic RAM.


* `write8(addr, value)`
* `write16(addr, value)`
* `write32(addr, value)`

Each of these functions takes an address (word) and a value (byte,
halfword, or word for `8`, `16`, and `32` respectively) to write to the
given location in memory. The memory address given may correspond to a
hardware I/O register or other unmapped memory, not necessarily only the
basic RAM.


## Register Access ##
Every emulator CPU register is assigned a positive index. The registers
may be accessed via this index using the `getreg` or `setreg` functions
below, or directly by name as in this example:

```lua
v0 = v0 + t0 * 5
```


* `getreg(index) => value`
* `setreg(index, value)`

Gets or sets the value of a register as an unsigned 32-bit integer. The
index identifies which register to access. The registers include GPR's,
HI/LO, PC, COP0 and GTE registers.


* `register_index_by_name`

**TBD**


* `register_name_by_index`

**TBD**


### GPR Register Indexes ###
| ID | Name | | ID | Name |
|---:|------|-|---:|------|
|   1|`r0`  | |  17|`s0`  |
|   2|`at`  | |  18|`s1`  |
|   3|`v0`  | |  19|`s2`  |
|   4|`v1`  | |  20|`s3`  |
|   5|`a0`  | |  21|`s4`  |
|   6|`a1`  | |  22|`s5`  |
|   7|`a2`  | |  23|`s6`  |
|   8|`a3`  | |  24|`s7`  |
|   9|`t0`  | |  25|`t8`  |
|  10|`t1`  | |  26|`t9`  |
|  11|`t2`  | |  27|`k0`  |
|  12|`t3`  | |  28|`k1`  |
|  13|`t4`  | |  29|`gp`  |
|  14|`t5`  | |  30|`sp`  |
|  15|`t6`  | |  31|`s8`  |
|  16|`t7`  | |  32|`ra`  |

Note that the GPR indexes are one off from the standard MIPS register
numbers, so for a given register <code>r<em>N</em></code>, the index is
`N + 1`.

GPR's have a few categories which indicate their general usage:

* `v` registers, for immediate values
* `a` registers, for arguments
* `t` registers, for temporaries
* `s` registers, for callee-saved temporaries
* `k` registers, for kernel (BIOS) use only
* `at`, for use by the assembler
* `r0`, hardwired to zero; **writes to this register are discarded**
* `gp`, usage may vary by game
* `sp`, stack pointer
* `ra`, return address

Generally, only the `sp`, `gp`, and `s` registers are preserved across
function calls. Most functions may trash the other registers, except for
`k0` and `k1` which should never be modified outside of system code.


### COP0 Register Indexes ###
**TBD**

Registers are named `cp0_r0` through `cp0_r31`.


### GTE (COP2) Register Indexes ###
**TBD**

Data registers are named `cp2d_r0` through `cp2d_r31`. Control registers
are named `cp2c_r0` through `cp2c_r31`.


### Other Register Indexes ###
| ID | Name | Purpose                                               |
|---:|------|-------------------------------------------------------|
|  33|`lo`  |Low-word output for MUL, quotient output for division  |
|  34|`hi`  |High-word output for MUL, remainder output for division|


| ID | Name | Purpose       |
|---:|------|---------------|
| 131|`pc`  |Program counter|


## Traps ##
* `trapexec(addr, func)`

Creates an execution trap at the given address with the given handler
function. During CPU operation, whenever an instruction fetch is about
to occur at the given address, the handler function will be called.

If multiple execution traps have been added to the same address, they
will be called in order from last-added to first-added. After all the
handlers have been called, the instruction located at PC will be fetched
and executed.

__Execution traps are not raised when executing instructions as part of
a branch delay slot.__


## Other ##
* `config`

This is a table of the user's configuration, including any defaults
which were not overwritten or otherwise removed.


* `_ENV` metatable

The script's environment (which is also the global environment `_G`)
has a metatable including metamethods `__index` and `__newindex`. These
metamethods should be preserved, or any replacement metamethods should
call out to these original metamethods when necessary. Failure to
preserve these may inhibit or disable some functionality, such as
register access by global.


* `ntsc` and `pal`

These are global variables set at startup. If the game region is NTSC
(U/C or J), then `ntsc` is `true` and `pal` is `false`. If the game
region is PAL, then `ntsc` is false and `pal` is `true`.

Changing the value of these variables has no effect on the region used
by the emulator.


## Continual Updating ##
Rather than returning, the script may yield (`coroutine.yield()`). In
this case, rather than exiting, the script continues to live suspended
in the background, and will occasionally be resumed.

Execution of the emulator and the script is never parallel; the CPU is
suspended whenever the script is resumed, and the CPU will not resume
execution until the script yields or returns.

If an error is raised unprotected in the script after the first yield,
the script will be terminated and will not be resumed again. Traps will
continue to function normally, however.
