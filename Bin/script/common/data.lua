local _data = {}

_data.GetOptionString = function(name)
    return KFrame:GetOptionString(name, "")
end

_data.GetOptionInt = function(name)
    return KFrame:GetOptionUint32(name, "")
end

_data.GetValue = function(playerid, key)
    return KFrame:GetValue(playerid, key)
end

_data.GetRecordValue = function(playerid, record_name, index, key)
    if key == nil then
        key = ""
    end
    return KFrame:GetRecordValue( playerid, record_name,  index, key)
end

_data.OperateRecordValue = function(playerid, record_name, index, key, op_type, value)
    KFrame:OperateRecordValue(playerid, record_name, index, key, op_type, value)
end

return _data