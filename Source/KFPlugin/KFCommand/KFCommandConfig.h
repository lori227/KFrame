#ifndef __KF_COMMAND_CONFIG_H__
#define __KF_COMMAND_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
	class KFCommandConfig : public KFConfig, public KFSingleton< KFCommandConfig >
	{
	public:
		KFCommandConfig();
		~KFCommandConfig();

		bool LoadConfig( const char* file );

	public:
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	static auto _kf_command_config = KFCommandConfig::Instance();
	//////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif