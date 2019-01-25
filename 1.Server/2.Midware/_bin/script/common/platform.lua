local _platform = {}

-- appid和appkey暂时只用来做平台web端的api使用
_platform.LW_SGAME_APP_ID = 10001 -- 江湖求生雷武平台appid
_platform.LW_SGAME_APP_KEY = "cHPNaYS27Ct4JZVF" -- 江湖求生雷武平台appkey

_platform.LW_CHANNEL = 0          -- 雷武正式渠道(全渠道可用)
_platform.LW_TEST_CHANNEL = 1     -- 雷武测试(内部)渠道
_platform.WX_CHANNEL = 2          -- 微信渠道
_platform.STEAM_CHANNEL = 3       -- steam渠道

return _platform