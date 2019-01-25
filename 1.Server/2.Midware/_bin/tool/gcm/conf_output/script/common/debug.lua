local log = require("script.common.log")

local _debug = {}

_debug.DebugVar = function(tbl, exclusions)
    local nests = 0
    if not exclusions then exclusions = {} end
    local recurse = function(t, recurse, exclusions)
        local indent = function()
            local indent_str = ""
            for ii = 1, nests do
                indent_str = indent_str .. "    "
            end
            return indent_str
        end

        local excluded = function(key)
            for k,v in pairs(exclusions) do
                if v == key then
                    return true
                end
            end
            return false
        end
        local isFirst = true
        for k,v in pairs(t) do
            if isFirst then
                log.LogDebug(indent() .. "|")
                isFirst = false
            end
            if type(v) == "table" and not excluded(k) then
                log.LogDebug(indent() .. "|-> "..tostring(k)..": "..type(v))
                nests = nests + 1
                recurse(v, recurse, exclusions)
            elseif excluded(k) then
                log.LogDebug(indent() .. "|-> "..tostring(k)..": "..type(v))
            elseif type(v) == "userdata" or type(v) == "function" then
                log.LogDebug(indent() .. "|-> "..tostring(k)..": "..type(v).." > "..tostring(v))
            elseif type(v) == "string" then
                log.LogDebug(indent() .. "|-> "..tostring(k)..": ".."\""..v.."\"")
            else
                log.LogDebug(indent() .. "|-> "..tostring(k)..": "..v)
            end
        end
        nests = nests - 1
    end

    nests = 0
    log.LogDebug("### START DUMP ###")
    local t = { tbl }
    for k,v in pairs(t) do
        if type(v) == "table" then
            log.LogDebug("|-> "..tostring(k)..": "..type(v))
            nests = nests + 1
            recurse(v, recurse, exclusions)
        elseif type(v) == "userdata" or type(v) == "function" then
            log.LogDebug("|-> "..tostring(k)..": "..type(v).." > "..tostring(v))
        elseif type(v) == "string" then
            log.LogDebug("|-> "..tostring(k)..": ".."\""..v.."\"")
        else
            log.LogDebug("|-> "..tostring(k)..": "..v)
        end
    end
    log.LogDebug("### END EUMP ###")
end

return _debug