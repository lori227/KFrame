# Lua脚本接口


## 主要功能
	
	所有的接口都在KFLua插件中导出, 供lua脚本使用

## 接口列表

### LogDebug
	
	打印Debug日志

	参数: 
	data : 日志内容

	用例 : 
	KFrame:LogDebug( "log test" )
	
### LogInfo
	
	打印Info日志

	参数: 
	data : 日志内容

	用例 : 
	KFrame:LogInfo( "log test" )

### LogWarn
	
	打印Warn日志

	参数: 
	data : 日志内容

	用例 : 
	KFrame:LogWarn( "log test" )

### LogError
	
	打印Error日志

	参数: 
	data : 日志内容

	用例 : 
	KFrame:LogError( "log test" )

### Md5Encode
	
	获得数据的md5值

	参数: 
	data : 数据内容

	用例 : 
	local md5data = KFrame:Md5Encode( "content" )

### GetValue
	
	获得玩家的一级属性( 如: money, level等 )

	参数: 
	objectid : 玩家id
	data_name : 属性名称

	用例 : 
	local money = KFrame:GetValue( 1001, "money" )


### OperateValue
	
	操作(加, 减, 赋值 )玩家的一级属性( 如: money, level等 )

	参数: 
	objectid : 玩家id
	data_name : 属性名称
	operate : 操作类型( 1=加, 2=减, 3=设置, 其他请参考KFEnum枚举值 )
	value : 操作的属性值

	返回值 : 最新的属性值
	
	用例 : 
	KFrame:OperateValue( 1001, "money", 1, 1000 )


### GetObjectValue
	
	获得玩家的二级属性( 如: sign->day )

	参数: 
	objectid : 玩家id
	parent_name : 父属性名称
	data_name : 子属性名称

	用例 : 
	local dayvalue = KFrame:GetObjectValue( 1001, "sign", "day" )

### OperateObjectValue

	操作(加, 减, 赋值 )玩家的二级属性( 如: sign->day )

	参数: 
	objectid : 玩家id
	parent_name : 父属性名称
	data_name : 属性名称
	operate : 操作类型( 1=加, 2=减, 3=设置, 其他请参考KFEnum枚举值 )
	value : 操作的属性值	

	返回值 : 最新的属性值
	
	用例 : 
	KFrame:OperateObjectValue( 1001, "sign", "day", 1, 1000 )

### GetRecordValue
	
	获得玩家的集合(三级)属性值( 如: item->1->count )

	参数: 
	objectid : 玩家id
	parent_name : 父属性名称
	key : 属性的key值
	data_name : 子属性名称

	用例 : 
	local count = KFrame:GetRecordValue( 1001, "item", 1, "count" )


### OperateRecordValue
	
	操作(加, 减, 赋值 )玩家的集合(三级)属性属性( 如: item->1->count )

	参数: 
	objectid : 玩家id
	parent_name : 父属性名称
	key : 属性的key值
	data_name : 属性名称
	operate : 操作类型( 1=加, 2=减, 3=设置, 其他请参考KFEnum枚举值 )
	value : 操作的属性值	

	返回值 : 
	最新的属性值
	
	用例 : 
	KFrame:OperateRecordValue( 1001, "item", 1, "count", 1, 2 )

### GetVariable
	
	获得lua脚本临时变量值( 所有脚本通用 )

	参数: 
	varid : 变量

	用例 : 
	local value = KFrame:GetVariable( 1 )

### OperateVariable
	
	操作(加, 减, 赋值 )lua的临时变量值( 所有脚本通用 )

	参数: 
	varid : 变量
	operate : 操作类型( 1=加, 2=减, 3=设置, 其他请参考KFEnum枚举值 )
	value : 操作的属性值	

	返回值 : 最新的变量值
	
	用例 : 
	KFrame:OperateVariable( 1, 1, 2 )

### GetGameTime
	
	获得游戏时间

	用例 : local time = KFrame:GetGameTime()

### GetRealTime
	
	获得现实时间( 时间戳 )

	用例 : 
	local time = KFrame:GetRealTime()

### AddData
	
	添加元数据( 奖励 )	

	参数: 
	objectid : 玩家id
	strelement : 需要添加的元数据字符串
	showclient : 是否通知客户端显示
	
	用例 : 
    local addhero = "[{\"hero\":{\"id\":\"1\"}},{\"hero\":{\"id\":\"2\"}}]"
	KFrame:AddData( 1001, addhero, false )


### SetData
	
	设置元数据

	参数: 
	objectid : 玩家id
	strelement : 需要添加的元数据字符串
	showclient : 是否通知客户端显示
	
	用例 : 
    local setmoney = "[{\"money\":\"2000\"}]"
	KFrame:SetData( 1001, setmoney, false )


### DecData
	
	设置元数据

	参数: 
	objectid : 玩家id
	strelement : 需要添加的元数据字符串
	
	用例 : 
    local decmoney = "[{\"money\":\"2000\"}]"
	KFrame:DecData( 1001, decmoney )


### STHttpGet
	
	同步, 使用get方式请求http

	参数: 
	url : http访问地址
	data : 发送的数据

	返回值:
	http访问的结果字串
	
	用例 : 
	local recvdata = KFrame:STHttpGet( "http://127.0.0.1/test", "test" )

### STHttpPost
	
	同步, 使用post方式请求http

	参数: 
	url : http访问地址
	data : 发送的数据

	返回值:
	http访问的结果字串
	
	用例 : 
	local recvdata = KFrame:STHttpPost( "http://127.0.0.1/test", "test" )


### MTHttpGet
	
	异步, 使用get方式请求http

	参数: 
	url : http访问地址
	data : 发送的数据
	args : 回调参数( json格式, 需要playerid,luafile, luafunction, 如没有luafile和luafunction则不会执行回调 )
		  不需要回调直接传入空字串
		
	用例 : 
	local args = {"playerid":1001,"luafile":"../script/game/test.lua","luafunction":"testfunction"}
	KFrame:MTHttpGet( "http://127.0.0.1/test", "test", args )

### LuaMTPost
	
	异步, 使用post方式请求http

	参数: 
	url : http访问地址
	data : 发送的数据
	args : 回调参数( json格式, 需要playerid,luafile, luafunction,如没有luafile和luafunction则不会执行回调 )
		  不需要回调直接传入空字串

	用例 : 
	local args = {"playerid":1001,"luafile":"../script/game/test.lua","luafunction":"testfunction"}
	KFrame:LuaMTPost( "http://127.0.0.1/test", "test", args )

### GetOptionString
	
	获得全局配置option的string值

	参数: 
	name : 配置名称
	logicid : 逻辑id( 默认为"" )
	
	用例 : 
	local strvalue = KFrame:GetOptionString( "routename", "" )

### GetOptionUInt32
	
	获得全局配置option的uint32值

	参数: 
	name : 配置名称
	logicid : 逻辑id( 默认为"" )
	
	用例 : 
	local intvalue = KFrame:GetOptionUInt32( "datasavetime", "" )