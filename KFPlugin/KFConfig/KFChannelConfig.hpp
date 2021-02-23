#ifndef	__KF_CHANNEL_CONFIG_H__
#define	__KF_CHANNEL_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFChannelSetting : public KFIntSetting
	{
	public:
		// 支持的渠道
		UInt32Set _support;

		// Debug
		uint32 _debug = 0u;

		// Release
		uint32 _release = 0u;

		// AppId
		std::string _app_id;

		// AppKey
		std::string _app_key;

		// LoginUrl
		std::string _login_url;

		// PayUrl
		std::string _pay_url;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFChannelConfig : public KFConfigT<KFChannelSetting>, public KFInstance<KFChannelConfig>
	{
	public:
		KFChannelConfig()
		{
			_key_name = "id";
			_file_name = "channel";
		}

		~KFChannelConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFChannelSetting> setting )
		{
			setting->_support = xml_node.ReadUInt32Set( "support", true );
			setting->_debug = xml_node.ReadUInt32( "debug", true );
			setting->_release = xml_node.ReadUInt32( "release", true );
			setting->_app_id = xml_node.ReadString( "appid", true );
			setting->_app_key = xml_node.ReadString( "appkey", true );
			setting->_login_url = xml_node.ReadString( "loginurl", true );
			setting->_pay_url = xml_node.ReadString( "payurl", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
