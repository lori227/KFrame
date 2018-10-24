local json = require("script.lib.json")

function TestFunction( playerid )

    local t = {};
    t["account"] = "lori227"
    t["channel"] = 1
    t["playerid"] = playerid
    t["luafile"] = "./script/test.lua"
    t["luafunction"]="CallBackFunction"
    local data1 = json.encode( t )
    print( data )

    --local data1= "{\"account\":\"lori227\",\"channel\":1}"
    local result = KFrame:MTHttpClient( "http://192.168.10.230:7777/auth", data1 )
    print(result)

end

function CallBackFunction( playerid, senddata, recvdata )
    print( senddata )
    print( recvdata )
end