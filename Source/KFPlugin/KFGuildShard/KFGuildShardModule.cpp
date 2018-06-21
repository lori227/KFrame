#include "KFGuildShardModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __USE_WORKER__
#define __GUILD_REDIS_DRIVER__ _kf_redis->CreateExecute( KFField::_guild )
	
#ifdef __USE_WORKER__
#define __REGISTER_GUILD_MESSAGE__ __REGISTER_WORKER_MESSAGE__
#define __UNREGISTER_GUILD_MESSAGE__ __UNREGISTER_WORKER_MESSAGE__
#define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_worker->SendMessageToClient( kfguid, msgid, message )
#else
#define __REGISTER_GUILD_MESSAGE__ __REGISTER_MESSAGE__
#define __UNREGISTER_GUILD_MESSAGE__ __UNREGISTER_MESSAGE__
#define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_cluster_shard->SendMessageToClient( kfguid, msgid, message )
#endif

	KFGuildShardModule::KFGuildShardModule()
	{

	}

	KFGuildShardModule::~KFGuildShardModule()
	{

	}

	void KFGuildShardModule::InitModule()
	{
	}

	void KFGuildShardModule::BeforeRun()
	{
			}
	
	void KFGuildShardModule::BeforeShut()
	{
	
	}

	void KFGuildShardModule::OnceRun()
	{
	
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////
}