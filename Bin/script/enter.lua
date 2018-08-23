function NewPlayerFunction( playerid )
	-- 添加默认的模型

	local adddata = "[{\"item\":{\"id\":\"3\"|\"count\":\"1\"}}|{\"item\":{\"id\":\"7\"|\"count\":\"1\"}}|{\"item\":{\"id\":\"8\"|\"count\":\"1\"}}]"
	KFrame:AddData( "player", playerid, adddata, false );
end