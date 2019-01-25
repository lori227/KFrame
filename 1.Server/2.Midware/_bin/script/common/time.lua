local _time = {}

_time.MinSec = 60
_time.HourSec = 3600
_time.DaySec = 86400
_time.WeekSec = 604800

_time.Now = function ()
    return KFrame:GetRealTime()
end

return _time