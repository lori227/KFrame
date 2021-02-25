#ifndef	__KF_LEAVE_CONFIG_H__
#define	__KF_LEAVE_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFLeaveSetting : public KFIntSetting
	{
	public:
		// lua脚本
		std::string _lua_file;

		// 调用函数
		std::string _lua_function;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFLeaveConfig : public KFConfigT<KFLeaveSetting>, public KFInstance<KFLeaveConfig>
	{
	public:
		KFLeaveConfig()
		{
			_key_name = "id";
			_file_name = "leave";
		}

		~KFLeaveConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFLeaveSetting> setting )
		{
			setting->_lua_file = xml_node.ReadString( "luafile", true );
			setting->_lua_function = xml_node.ReadString( "luafunction", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
