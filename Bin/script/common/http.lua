local json = require("script.common.json")
local log = require("script.common.log")
local md5 = require("script.common.md5")
local debug = require("script.common.debug")

local _http = {}
-------------------------------
-- Function: EncodeUrlParam
-- Desc: url网址参数编码
-- Param: param表示要编码的参数table
-- Return: 编码后的字符串
-------------------------------
_http.EncodeUrlParam = function (param)
    local param_str = "?"
    for key, value in pairs(param) do
        param_str = param_str .. key .. "=" .. tostring(value) .. "&"
    end
    -- 去掉最后一个&
    return string.sub(param_str, 1, string.len(param_str) - 1)
end

_http.StartHttpClient = function (url, body, args)
    local body_str = json.JsonEncode(body)

    local args_str = ""
    if args ~= nil then
        args_str = json.JsonEncode(args)
    end

    log.LogInfo("url=" .. url .. " body=" .. body_str .. " args=" .. args_str)
    KFrame:MTHttpClient(url, body_str, args_str)
end

_http.CommonSig = function (timestamp)
    return KFrame:MakePlatformSign(timestamp)
end

_http.PlatformUrl = function( path )
    return KFrame:MakePlatformUrl(path)
end

return _http