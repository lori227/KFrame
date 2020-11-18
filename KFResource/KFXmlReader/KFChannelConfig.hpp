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
	class KFChannelConfig : public KFConfigT< KFChannelSetting >, public KFInstance< KFChannelConfig >
	{
	public:
		KFChannelConfig()
		{
			_key_name = "id";
			_file_name = "channel";
		}

		~KFChannelConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFChannelSetting* kfsetting )
		{
			kfsetting->_support = xmlnode.ReadUInt32Set( "support", true );
			kfsetting->_debug = xmlnode.ReadUInt32( "debug", true );
			kfsetting->_release = xmlnode.ReadUInt32( "release", true );
			kfsetting->_app_id = xmlnode.ReadString( "appid", true );
			kfsetting->_app_key = xmlnode.ReadString( "appkey", true );
			kfsetting->_login_url = xmlnode.ReadString( "loginurl", true );
			kfsetting->_pay_url = xmlnode.ReadString( "payurl", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
