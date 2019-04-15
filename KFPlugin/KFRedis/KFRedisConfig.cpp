#include "KFRedisConfig.hpp"

namespace KFrame
{
    KFRedisList* KFRedisType::FindRedisList( uint32 type )
    {
        return _redis_list.Find( type );
    }

    KFRedisList* KFRedisType::AddRedisList( uint32 type )
    {
        return _redis_list.Create( type );
    }
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    void KFRedisList::Reset()
    {
        _redis_list.clear();
        _kfseting = nullptr;
    }

    void KFRedisList::AddSetting( KFRedisSetting& kfsetting )
    {
        _redis_list.push_back( kfsetting );
    }

    const KFRedisSetting* KFRedisList::FindSetting()
    {
        if ( _kfseting == nullptr )
        {
            auto index = KFGlobal::Instance()->_app_id->GetId() % static_cast< uint32 >( _redis_list.size() );
            _kfseting = &_redis_list[ index ];
        }

        return _kfseting;
    }

    KFRedisType* KFRedisConfig::FindRedisType( const std::string& module, uint32 logicid )
    {
        auto key = ModuleKey( module, logicid );
        return _redis_type.Find( key );
    }
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    bool KFRedisConfig::LoadConfig()
    {
        auto _redis_id = 0u;
        _redis_type.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();

        auto modulenode = config.FindNode( "Module" );
        while ( modulenode.IsValid() )
        {
            auto name = modulenode.GetString( "Name" );

            auto redisnode = modulenode.FindNode( "Redis" );
            while ( redisnode.IsValid() )
            {
                auto minlogicid = redisnode.GetUInt32( "MinLogicId" );
                auto maxlogicid = redisnode.GetUInt32( "MaxLogicId" );

                for ( auto i = minlogicid; i <= maxlogicid; ++i )
                {
                    auto kfredistype = _redis_type.Create( ModuleKey( name, i ) );
                    kfredistype->_id = ++_redis_id;

                    // write
                    auto writenode = redisnode.FindNode( "Write" );
                    while ( writenode.IsValid() )
                    {
                        KFRedisSetting kfsetting;

                        kfsetting._name = name;
                        kfsetting._type = KFDatabaseEnum::Write;
                        kfsetting._ip = writenode.GetString( "IP" );
                        kfsetting._port = writenode.GetUInt32( "Port" );
                        kfsetting._password = writenode.GetString( "Password" );

                        auto kfredislist = kfredistype->AddRedisList( KFDatabaseEnum::Write );
                        kfredislist->AddSetting( kfsetting );

                        writenode.NextNode( "Write" );
                    }

                    // read
                    auto readnode = redisnode.FindNode( "Read" );
                    while ( readnode.IsValid() )
                    {

                        KFRedisSetting kfsetting;
                        kfsetting._name = name;
                        kfsetting._type = KFDatabaseEnum::Read;
                        kfsetting._ip = readnode.GetString( "IP" );
                        kfsetting._port = readnode.GetUInt32( "Port" );
                        kfsetting._password = readnode.GetString( "Password" );

                        auto kfredislist = kfredistype->AddRedisList( KFDatabaseEnum::Read );
                        kfredislist->AddSetting( kfsetting );

                        readnode.NextNode( "Read" );
                    }
                }

                redisnode.NextNode();
            }

            modulenode.NextNode();
        }

        return true;
    }
}