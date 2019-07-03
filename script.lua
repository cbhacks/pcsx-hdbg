print '                                                                       '
print ' PCSX-HDBG - PCSX-based hack debugger                                  '
print ' Copyright (C) 2019  "chekwob" <chek@wobbyworks.com>                   '
print '                                                                       '
print ' Based on PCSX-r, PCSX-df, PCSX, and the accompanying emulator GPU,    '
print ' SPU, and PAD plugins. These are very large projects with too many     '
print ' contributors to list here. Please see the include AUTHORS file for    '
print ' more details.                                                         '
print '                                                                       '
print ' This program is free software: you can redistribute it and/or modify  '
print ' it under the terms of the GNU General Public License as published by  '
print ' the Free Software Foundation, either version 3 of the License, or     '
print ' (at your option) any later version.                                   '
print '                                                                       '
print ' This program is distributed in the hope that it will be useful,       '
print ' but WITHOUT ANY WARRANTY; without even the implied warranty of        '
print ' MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         '
print ' GNU General Public License for more details.                          '
print '                                                                       '
print ' You should have received a copy of the GNU General Public License     '
print ' along with this program.  If not, see <http://www.gnu.org/licenses/>. '
print '                                                                       '

-- Setup the metatable for _ENV.
local META_ENV = {}
setmetatable(_ENV, META_ENV)

-- Raise errors instead of yielding nil when accessing globals which do
-- not exist or have not yet been initialized.
META_ENV.__index = function(t, k)
    error("no such global variable: " .. k)
end
META_ENV.__newindex = function(t, k, v)
    rawset(_ENV, k, v)
end

local register_name_by_index = {
    -- General purpose registers
    "r0", "at", "v0", "v1",
    "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3",
    "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3",
    "s4", "s5", "s6", "s7",
    "t8", "t9", "k0", "k1",
    "gp", "sp", "s8", "ra",

    -- MUL/DIV result registers
    "lo", "hi",

    -- COP0 registers
    "cp0_r0",
    "cp0_r1",
    "cp0_r2",
    "cp0_r3",
    "cp0_r4",
    "cp0_r5",
    "cp0_r6",
    "cp0_r7",
    "cp0_r8",
    "cp0_r9",
    "cp0_r10",
    "cp0_r11",
    "cp0_r12",
    "cp0_r13",
    "cp0_r14",
    "cp0_r15",
    "cp0_r16",
    "cp0_r17",
    "cp0_r18",
    "cp0_r19",
    "cp0_r20",
    "cp0_r21",
    "cp0_r22",
    "cp0_r23",
    "cp0_r24",
    "cp0_r25",
    "cp0_r26",
    "cp0_r27",
    "cp0_r28",
    "cp0_r29",
    "cp0_r30",
    "cp0_r31",

    -- GTE data registers
    "cp2d_r0",
    "cp2d_r1",
    "cp2d_r2",
    "cp2d_r3",
    "cp2d_r4",
    "cp2d_r5",
    "cp2d_r6",
    "cp2d_r7",
    "cp2d_r8",
    "cp2d_r9",
    "cp2d_r10",
    "cp2d_r11",
    "cp2d_r12",
    "cp2d_r13",
    "cp2d_r14",
    "cp2d_r15",
    "cp2d_r16",
    "cp2d_r17",
    "cp2d_r18",
    "cp2d_r19",
    "cp2d_r20",
    "cp2d_r21",
    "cp2d_r22",
    "cp2d_r23",
    "cp2d_r24",
    "cp2d_r25",
    "cp2d_r26",
    "cp2d_r27",
    "cp2d_r28",
    "cp2d_r29",
    "cp2d_r30",
    "cp2d_r31",

    -- GTE control registers
    "cp2c_r0",
    "cp2c_r1",
    "cp2c_r2",
    "cp2c_r3",
    "cp2c_r4",
    "cp2c_r5",
    "cp2c_r6",
    "cp2c_r7",
    "cp2c_r8",
    "cp2c_r9",
    "cp2c_r10",
    "cp2c_r11",
    "cp2c_r12",
    "cp2c_r13",
    "cp2c_r14",
    "cp2c_r15",
    "cp2c_r16",
    "cp2c_r17",
    "cp2c_r18",
    "cp2c_r19",
    "cp2c_r20",
    "cp2c_r21",
    "cp2c_r22",
    "cp2c_r23",
    "cp2c_r24",
    "cp2c_r25",
    "cp2c_r26",
    "cp2c_r27",
    "cp2c_r28",
    "cp2c_r29",
    "cp2c_r30",
    "cp2c_r31",

    -- Program counter
    "pc"
}

local register_index_by_name = {}
for i, v in ipairs(register_name_by_index) do
    register_index_by_name[v] = i
end

do
    local old_index = META_ENV.__index
    local old_newindex = META_ENV.__newindex

    META_ENV.__index = function(t, k)
        local regnum = register_index_by_name[k]
        if regnum then
            return getreg(regnum)
        else
            return old_index(t, k)
        end
    end

    META_ENV.__newindex = function(t, k, v)
        local regnum = register_index_by_name[k]
        if regnum then
            setreg(regnum, v)
        else
            old_newindex(t, k, v)
        end
    end
end

function printf(...)
    print(string.format(...))
end

local eid_charset = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_!"
function eidtoname(value)
    local char1 = ((value >> 25) & 0x3F) + 1
    local char2 = ((value >> 19) & 0x3F) + 1
    local char3 = ((value >> 13) & 0x3F) + 1
    local char4 = ((value >> 7) & 0x3F) + 1
    local char5 = ((value >> 1) & 0x3F) + 1

    char1 = eid_charset:sub(char1, char1)
    char2 = eid_charset:sub(char2, char2)
    char3 = eid_charset:sub(char3, char3)
    char4 = eid_charset:sub(char4, char4)
    char5 = eid_charset:sub(char5, char5)

    return char1 .. char2 .. char3 .. char4 .. char5
end

print "Hello, world!"

-- Example: Change starting level of NTSC-U Crash 2 to Ruination
write8(0x800117E4, 0xF)

-- Example: Make GOOL RNG always return the lower-bound input
local rng_lastpc
local rng_counter
trapexec(0x8003A3B4, function()
    if rng_lastpc ~= s5 then
        rng_lastpc = s5
        rng_counter = 0
    end
    rng_counter = rng_counter + 1

    if rng_counter <= 20 then
        v1 = 0
    else
        -- In case of infinite loop on RNG instructions (spitting plants,
        -- Diggin' It statues, Tiny boss fight, etc), start returning increasing
        -- RNG values until the next different-PC RNG instruction.
        v1 = rng_counter - 20
    end
end)

-- Example: Print chunk load/unload messages
trapexec(0x800125F8, function()
    local chunkslot = a0
    local chunkinfo_p = a0 * 44 + 0x800675B4
    local chunk_p = readu32(chunkinfo_p)
    local chunk_type = readu16(chunk_p + 2)
    local chunk_id = readu32(chunk_p + 4)
    if chunk_type == 1 then
        chunk_id = eidtoname(chunk_id)
    end
    printf("CHUNK   LOAD @ slot %2d: chunk T%d; id %s", chunkslot, chunk_type, chunk_id)
end)
trapexec(0x80012A20, function()
    local chunkslot = (a0 - 0x800675B4) // 44
    local chunkinfo_p = a0
    local chunk_p = readu32(chunkinfo_p)
    local chunk_type = readu16(chunk_p + 2)
    local chunk_id = readu32(chunk_p + 4)
    if chunk_type == 1 then
        chunk_id = eidtoname(chunk_id)
    end
    printf("CHUNK UNLOAD @ slot %2d: chunk T%d; id %s", chunkslot, chunk_type, chunk_id)
end)
trapexec(0x800126F4, function()
    --print("LOADING ENTRY: " .. eidtoname(v0))
end)