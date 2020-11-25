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
        _kf_seting = nullptr;
    }

    void KFRedisList::AddSetting( KFRedisSetting& kfsetting )
    {
        _redis_list.push_back( kfsetting );
    }

    const KFRedisSetting* KFRedisList::FindSetting()
    {
        if ( _kf_seting == nullptr )
        {
            auto index = KFGlobal::Instance()->_app_id->GetId() % static_cast< uint32 >( _redis_list.size() );
            _kf_seting = &_redis_list[ index ];
        }

        return _kf_seting;
    }

    KFRedisType* KFRedisConfig::FindRedisType( const std::string& module, uint32 logicid )
    {
        auto key = ModuleKey( module, logicid );
        auto redistype = _redis_type.Find( key );
        if ( redistype == nullptr )
        {
            redistype = _redis_type.First();
        }

        return redistype;
    }
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    bool KFRedisConfig::LoadConfig( const std::string& filepath, uint32 cleartype )
    {
        auto _redis_id = 0u;
        _redis_type.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( filepath );
        auto config = kfxml.RootNode();

        auto modulenode = config.FindNode( "Module" );
        while ( modulenode.IsValid() )
        {
            auto name = modulenode.ReadString( "Name" );

            auto redisnode = modulenode.FindNode( "Redis" );
            while ( redisnode.IsValid() )
            {
                auto minlogicid = redisnode.ReadUInt32( "MinLogicId" );
                auto maxlogicid = redisnode.ReadUInt32( "MaxLogicId" );

                for ( auto i = minlogicid; i <= maxlogicid; ++i )
                {
                    auto kfredistype = _redis_type.Create( ModuleKey( name, i ) );
                    kfredistype->_id = ++_redis_id;

                    // write
                    auto writenode = redisnode.FindNode( "Write" );
                    while ( writenode.IsValid() )
                    {
                        KFRedisSetting kfsetting;

                        kfsetting._module = name;
                        kfsetting._ip = writenode.ReadString( "IP" );
                        kfsetting._port = writenode.ReadUInt32( "Port" );
                        kfsetting._password = writenode.ReadString( "Password" );

                        auto kfredislist = kfredistype->AddRedisList( KFDatabaseEnum::Write );
                        kfredislist->AddSetting( kfsetting );

                        writenode.NextNode( "Write" );
                    }

                    // read
                    auto readnode = redisnode.FindNode( "Read" );
                    while ( readnode.IsValid() )
                    {

                        KFRedisSetting kfsetting;
                        kfsetting._module = name;
                        kfsetting._ip = readnode.ReadString( "IP" );
                        kfsetting._port = readnode.ReadUInt32( "Port" );
                        kfsetting._password = readnode.ReadString( "Password" );

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