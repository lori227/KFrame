#include "KFPlatformConfig.h"

namespace KFrame
{
    bool KFPlatformConfig::LoadConfig()
    {
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "Platform" );
        while ( xmlnode.IsValid() )
        {
            auto channel = xmlnode.GetUInt32( "Channel" );
            if ( KFGlobal::Instance()->CheckChannelService( channel, _invalid_int ) )
            {
                _app_id = xmlnode.GetString( "AppId" );
                _app_key = xmlnode.GetString( "AppKey" );
                _app_url = xmlnode.GetString( "AppUrl" );

                break;
            }

            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////
        return true;
    }
}