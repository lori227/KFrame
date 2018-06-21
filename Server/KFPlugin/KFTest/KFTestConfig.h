#ifndef __KF_TEST_CONFIG_H__
#define __KF_TEST_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
	namespace KFTestEnum
	{
	}


	class KFTestConfig : public KFConfig, public ISingleton< KFTestConfig >
	{
	public:
		KFTestConfig();
		~KFTestConfig();

		bool LoadConfig( const char* file );

	public:
		bool _is_client;
		uint32 _client_count;
		uint32 _message_count;

	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	static auto* _kf_test_config = KFTestConfig::Instance();
	//////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif