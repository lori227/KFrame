#include "KFNetPort.h"


namespace KFrame
{

    KFPort* KFramePort::FindPort( uint32 id )
    {
        KFLocker locker( _kf_mutex );

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
                kfport->_port = 1000 + i + 1;
                return kfport;
            }
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    KFNetProt::KFNetProt()
    {
    }

    KFNetProt::~KFNetProt()
    {

    }

    uint32 KFNetProt::CalcListenPort( uint32 type, uint32 appid, uint32 baseport )
    {
        uint32 port = baseport;
        if ( type == __ID_PORT__ )
        {
            port = baseport + appid;
        }
        else if ( type == __SH_PORT__ )
        {
            // 共享内存结算
            auto address = _kf_share_memory.CreateMemory( KFField::_port, 4096 );

            auto kframeport = reinterpret_cast< KFramePort* >( address );
            if ( kframeport->_is_inited == 0 )
            {
                kframeport = new ( address ) KFramePort();
                kframeport->_is_inited = 1;
            }

            // 查找自己是否在列表中
            auto kfport = kframeport->FindPort( appid );
            port = baseport + kfport->_port;
        }

        return port;
    }

}
