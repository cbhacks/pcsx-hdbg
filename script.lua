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

local c2_scus = {
    chunkinfos = 0x800675B4,
    chunkloadfn = 0x800125F8,
    chunkunloadfn = 0x80012A20
}

local c2_sces = {
    chunkinfos = 0x8006784C,
    chunkloadfn = 0x800126BC,
    chunkunloadfn = 0x80012AE4
}

local game
if ntsc then
    game = c2_scus
else
    game = c2_sces
end

-- Example: Print chunk load/unload messages
trapexec(game.chunkloadfn, function()
    local chunkslot = a0
    local chunkinfo_p = a0 * 44 + game.chunkinfos
    local chunk_p = readu32(chunkinfo_p)
    local chunk_type = readu16(chunk_p + 2)
    local chunk_id = readu32(chunk_p + 4)
    if chunk_type == 1 then
        chunk_id = eidtoname(chunk_id)
    end
    printf("CHUNK   LOAD @ slot %2d: chunk T%d; id %s", chunkslot, chunk_type, chunk_id)
end)
trapexec(game.chunkunloadfn, function()
    local chunkslot = (a0 - game.chunkinfos) // 44
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
