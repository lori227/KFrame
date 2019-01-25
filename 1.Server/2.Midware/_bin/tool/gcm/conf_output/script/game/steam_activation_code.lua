local platform = require("script.common.platform")
local log = require("script.common.log")
local time = require("script.common.time")
local json = require("script.common.json")
local http = require("script.common.http")
local debug = require("script.common.debug")
local data = require("script.common.data")

function GetSteamActivationCode( playerid )
    local grant_value = data.GetRecordValue( playerid, "note",  20001, "value")
    if grant_value == 1 then
        return
    end

    -- 获取在线时长
    local total_online_time = data.GetValue(playerid, "totalonlinetime")
    if total_online_time < 3 * time.HourSec then -- 3小时
        return
    end

    -- 获取渠道
    local channel = data.GetValue(playerid, "channel")

    -- post data
    local http_body = {}
    http_body["accountid"] = playerid
    http_body["channel"] = channel
    
    -- callback args
    local arg = {}
    arg["playerid"] = playerid
    arg["luafile"] = "./script/game/steam_activation_code.lua"
    arg["luafunction"]="HttpCallBackFunction"

    -- 获取url
    local platformurl = http.MakePlatformUrl("getactivationcode")
    http.StartHttpClient( platformurl, http_body, arg )
end

function HttpCallBackFunction( playerid, senddata, recvdata )
    local recv_json = json.JsonDecode(recvdata)
    if recv_json["code"] ~= 0 then
        log.LogError("GetSteamActivationCode callback, playerid=" .. tostring(playerid) .. " send_data=" .. senddata .. " recv_data=" .. recvdata)
        return
    end
    log.LogInfo("GetSteamActivationCode callback, playerid=" .. tostring(playerid) .. " send_data=" .. senddata .. " recv_data=" .. recvdata)

    data.OperateRecordValue( playerid, "note",  20001, "value", 3, 1)
end