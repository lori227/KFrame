-- 添加属性
function AddData( playerid, stragent )
	KFrame:AddData( "player", playerid, stragent, true );
end

-- 设置属性
function SetData( playerid, stragent )
	KFrame:SetData( "player", playerid, stragent, true );
end

-- 减少属性
function DecData( playerid, stragent )
	KFrame:DecData( "player", playerid, stragent );
end
	
function AddRecent( playerid, stragent )
	KFrame:AddRecent( "player", playerid, stragent );
end