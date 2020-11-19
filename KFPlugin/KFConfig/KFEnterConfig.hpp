#ifndef	__KF_ENTER_CONFIG_H__
#define	__KF_ENTER_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFEnterSetting : public KFIntSetting
	{
	public:
		// lua脚本
		std::string _lua_file;

		// 调用函数
		std::string _lua_function;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFEnterConfig : public KFConfigT< KFEnterSetting >, public KFInstance< KFEnterConfig >
	{
	public:
		KFEnterConfig()
		{
			_key_name = "id";
			_file_name = "enter";
		}

		~KFEnterConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFEnterSetting* kfsetting )
		{
			kfsetting->_lua_file = xmlnode.ReadString( "luafile", true );
			kfsetting->_lua_function = xmlnode.ReadString( "luafunction", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
