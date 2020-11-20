#ifndef	__KF_LOADER_CONFIG_H__
#define	__KF_LOADER_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	class KFConfigData
	{
	public:
		// 配置名
		std::string _name;

		// 配置文件路径
		std::string _path;

		// 主文件配置名(主文件重新加载后需要重新加载)
		std::string _parent_name;

		// 是否可以重新加载
		bool _can_reload = false;

		// 是否需要清空数据
		uint32 _clear_type = 0u;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFLoaderSetting : public KFStrSetting
	{
	public:
		// 配置文件列表
		std::vector<KFConfigData> _config_data;
	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFLoaderConfig : public KFConfigT< KFLoaderSetting >, public KFInstance< KFLoaderConfig >
	{
	public:
		KFLoaderConfig()
		{
			_key_name = "id";
			_file_name = "loader";
		}

		~KFLoaderConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFLoaderSetting* kfsetting )
		{
		
			KFConfigData configdata;
			configdata._name = xmlnode.ReadString( "name" );
			configdata._path = xmlnode.ReadChannelString( "path" );
			configdata._parent_name = xmlnode.ReadString( "parentname" );
			configdata._can_reload = xmlnode.ReadBoolen( "canreload" );
			configdata._clear_type = xmlnode.ReadUInt32( "cleartype" );
			kfsetting->_config_data.push_back( configdata );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
