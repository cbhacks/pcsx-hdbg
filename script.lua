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

print "Hello, world!"

-- Example: Change starting level of NTSC-U Crash 2 to Ruination
write8(0x800117E4, 0xF)

-- Example: Print a message on every NTSC-U Crash 2 GOOL RNG instruction
trapexec(0x8003A354, function()
    print "RNG"
end)
