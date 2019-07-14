--
-- PCSX-HDBG - PCSX-based hack debugger
-- Copyright (C) 2019  "chekwob" <chek@wobbyworks.com>
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.
--

-- Load and compile the user script.
local ok, script = pcall(loadfile, "script.lua", "t")
if not ok then
    print("Error loading 'script.lua':")
    print(script)
    os.exit(false)
end

-- Suspend here until the rest of the emulator is initialized.
coroutine.yield()

-- Display the program boot message.
print '                                                                       '
print ' PCSX-HDBG - PCSX-based hack debugger                                  '
print ' Copyright (C) 2019  "chekwob" <chek@wobbyworks.com>                   '
print '                                                                       '
print ' Based on PCSX-r, PCSX-df, PCSX, and the accompanying emulator GPU     '
print ' and SPU plugins. These are very large projects with too many          '
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

register_name_by_index = {
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

register_index_by_name = {}
for i, v in ipairs(register_name_by_index) do
    register_index_by_name[v] = i
end

-- Enable register access as global variables.
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

-- Run the user script.
print "Starting user script..."
local thread = coroutine.create(script)
local ok, err = coroutine.resume(thread)
if not ok then
    print("Error executing 'script.lua':")
    print(script)
    os.exit(false)
end
while coroutine.status(thread) ~= "dead" do
    local ok, err = coroutine.resume(thread)
    if not ok then
        print("Error doing update in user script:")
        print(script)
        print()
        print("User script updates have been terminated.")
        return
    end
    coroutine.yield()
end
print "User script finished OK."
