local json = require("script.lib.json")

local _json = {}

_json.JsonEncode = function(tbl)
    return json.encode(tbl)
end

_json.JsonDecode = function(tbl)
    return json.decode(tbl)
end

return _json