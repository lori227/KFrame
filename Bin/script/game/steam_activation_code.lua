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

    -- 获取url
    local url = data.GetOptionString("platapiurl").."getactivationcode"
    local nowtime = time.Now()

    -- param
    local url_param = {}
    url_param["timestamp"] = nowtime
    url_param["appid"] = data.GetOptionInt("platappid")
    url_param["sig"] = http.CommonSig(nowtime)

    -- post data
    local http_body = {}
    http_body["accountid"] = playerid
    http_body["channel"] = platform.LW_CHANNEL --发的激活码为通用渠道可用
    
    -- callback args
    local arg = {}
    arg["playerid"] = playerid
    arg["luafile"] = "./script/game/steam_activation_code.lua"
    arg["luafunction"]="HttpCallBackFunction"

    http.StartHttpClient( url, url_param, http_body, arg )
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