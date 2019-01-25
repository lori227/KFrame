local _log = {}

_log.LogDebug = function (str)
    KFrame:LogDebug(str)
end

_log.LogInfo = function (str)
    KFrame:LogInfo(str)
end

_log.LogWarn = function (str)
    KFrame:LogWarn(str)
end

_log.LogError = function (str)
    KFrame:LogError(str)
end

_log.LogCritical = function (str)
    KFrame:LogCritical(str)
end

return _log