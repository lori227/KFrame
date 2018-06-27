#ifndef __KF_FTP_CONFIG_H__
#define __KF_FTP_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
	__ST_CLASS__( KFFtpSetting )
	{
	public:
		KFFtpSetting()
		{
			_id = 0;
			_port = 21;
		}

	public:
		uint32 _id;
		std::string _address;
		uint32 _port;
		std::string _user;
		std::string _password;

		std::string _ftp_path;
		std::string _local_path;
	};

	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFFtpConfig : public KFConfig, public KFSingleton< KFFtpConfig >
	{
	public:
		KFFtpConfig();
		~KFFtpConfig();

		bool LoadConfig( const char* file );

		// 查找配置
		const KFFtpSetting* FindFtpSetting( uint32 id );

	public:
		KFMap< uint32, uint32, KFFtpSetting > _kf_ftp_setting;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	static auto _kf_ftp_config = KFFtpConfig::Instance();
	//////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif