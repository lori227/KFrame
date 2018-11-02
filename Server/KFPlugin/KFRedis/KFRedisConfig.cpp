#include "KFRedisConfig.h"

namespace KFrame
{
    void KFRedisList::Reset()
    {
        _index = 0;
        _redis_list.clear();
    }

    void KFRedisList::AddSetting( KFRedisSetting& kfsetting )
    {
        _redis_list.push_back( kfsetting );

        KFAppID kfappid( KFGlobal::Instance()->_app_id );
        if ( _redis_list.size() >= kfappid._union._app_data._instance_id )
        {
            _index = kfappid._union._app_data._instance_id - 1;
        }
        else
        {
            _index = kfappid._union._app_data._instance_id % _redis_list.size();
        }
    }

    const KFRedisSetting* KFRedisList::FindSetting()
    {
        if ( _index >= _redis_list.size() )
        {
            _index = 0;
        }

        return &_redis_list[ _index++ ];
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    const KFRedisSetting* KFRedisConfig::FindRedisSetting( const std::string& module, uint32 logicid )
    {
        auto key = ModuleKey( module, logicid );
        auto kfredislist = _redis_setting.Find( key );
        return kfredislist->FindSetting();
    }

    bool KFRedisConfig::LoadConfig()
    {
        auto _redis_id = 0u;
        _redis_setting.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto redisnode = config.FindNode( "Redis" );

        auto modulenode = redisnode.FindNode( "Connection" );
        while ( modulenode.IsValid() )
        {
            auto modulename = modulenode.GetString( "Module" );
            auto logicid = modulenode.GetUInt32( "LogicId", true, _invalid_int );
            auto kfredislist = _redis_setting.Create( ModuleKey( modulename, logicid ) );

            KFRedisSetting kfsetting;
            kfsetting._id = ++_redis_id;
            kfsetting._ip = modulenode.GetString( "IP" );
            kfsetting._port = modulenode.GetUInt32( "Port" );
            kfsetting._password = modulenode.GetString( "Password" );
            kfredislist->AddSetting( kfsetting );

            auto channelnode = modulenode.FindNode( "Channel" );
            while ( channelnode.IsValid() )
            {
                auto channelid = channelnode.GetUInt32( "ChannelId" );
                auto service = channelnode.GetUInt32( "Service" );
                if ( KFGlobal::Instance()->CheckChannelService( channelid, service ) )
                {
                    kfredislist->Reset();

                    auto datanode = channelnode.FindNode( "Data" );
                    while ( datanode.IsValid() )
                    {
                        KFRedisSetting kfsetting;
                        kfsetting._id = ++_redis_id;
                        kfsetting._ip = datanode.GetString( "IP" );
                        kfsetting._port = datanode.GetUInt32( "Port" );
                        kfsetting._password = datanode.GetString( "Password" );
                        kfredislist->AddSetting( kfsetting );

                        datanode.NextNode();
                    }
                    break;
                }

                channelnode.NextNode();
            }

            modulenode.NextNode();
        }

        return true;
    }
}