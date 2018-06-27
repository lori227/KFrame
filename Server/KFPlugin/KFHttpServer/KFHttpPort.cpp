#include "KFHttpPort.h"

namespace KFrame
{

	KFPort* KFramePort::FindPort( uint32 id )
	{
		KFLocker< KFMutex > locker( _kf_mutex );

		for ( auto i = 0; i < __MAX_PORT_COUNT__; ++i )
		{
			auto kfport = &_kf_port[ i ];
			if ( kfport->_id == id )
			{
				return kfport;
			}

			if ( kfport->_id == 0 )
			{
				kfport->_id = id;
				kfport->_port = i;
				return kfport;
			}
		}

		return nullptr;
	}

	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	KFHttpProt::KFHttpProt()
	{
	}

	KFHttpProt::~KFHttpProt()
	{

	}

	uint32 KFHttpProt::CalcListenPort( const std::string& appname, uint32 appid, uint32 baseport )
	{
		// 共享内存结算
		//auto address = _kf_share_memory.CreateMemory( appname, 4096 );

		//auto kframeport = reinterpret_cast< KFramePort* >( address );
		//if ( kframeport->_is_inited == 0 )
		//{
		//	kframeport = new ( address ) KFramePort();
		//	kframeport->_is_inited = 1;
		//}

		// 查找自己是否在列表中
		//auto kfport = kframeport->FindPort( appid );
		//auto port = baseport + kfport->_port;
		auto port = baseport + appid;
		return port;
	}

}
