#include "KFPlatformConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFPlatformConfig::KFPlatformConfig()
    {
        _token_expire_time = 0;
        _is_open_activation = false;
        _default_server_list_type = __TOTAL_SERVER_LIST__;
    }

    KFPlatformConfig::~KFPlatformConfig()
    {

    }

    bool KFPlatformConfig::LoadConfig( const char* file )
    {
        try
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();

            //////////////////////////////////////////////////////////////////
            auto platform = config.FindNode( "Platform" );
            _token_expire_time = platform.GetUInt32( "TokenExpireTime" );
            _is_open_activation = platform.GetBoolen( "OpenActivation" );

            auto serverlisttypes = config.FindNode( "ServerListTypes" );
            _default_server_list_type = serverlisttypes.GetUInt32( "Default" );

            auto serverlisttype = serverlisttypes.FindNode( "ServerListType" );
            while ( serverlisttype.IsValid() )
            {
                KFServerListType kftype;

                kftype._min_flag = serverlisttype.GetUInt32( "MinFlag" );
                kftype._max_flag = serverlisttype.GetUInt32( "MaxFlag" );
                kftype._list_type = serverlisttype.GetUInt32( "Type" );
                _server_list_type.push_back( kftype );

                serverlisttype.NextNode();
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }

    uint32 KFPlatformConfig::GetServerListType( uint32 appflag )
    {
        for ( auto& kftype : _server_list_type )
        {
            if ( appflag >= kftype._min_flag && appflag <= kftype._max_flag )
            {
                return kftype._list_type;
            }
        }

        return _default_server_list_type;
    }
}