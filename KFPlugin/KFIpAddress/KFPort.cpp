#include "KFPort.hpp"


namespace KFrame
{

    KFPort* KFramePort::FindPort( uint64 id )
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
                kfport->_port = i + 1;
                return kfport;
            }
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
}
