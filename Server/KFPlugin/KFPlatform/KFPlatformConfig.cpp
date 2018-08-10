#include "KFPlatformConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFPlatformConfig::KFPlatformConfig()
    {
        _token_expire_time = 0;
        _is_open_activation = false;
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
            auto serverlisttype = serverlisttypes.FindNode( "ServerListType" );
            while ( serverlisttype.IsValid() )
            {
                auto flag = serverlisttype.GetUInt32( "AppFlag" );
                auto type = serverlisttype.GetUInt32( "Type" );
                _server_list_type[ flag ] = type;

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
        auto iter = _server_list_type.find( appflag );
        if ( iter == _server_list_type.end() )
        {
            return _invalid_int;
        }

        return iter->second;
    }
}