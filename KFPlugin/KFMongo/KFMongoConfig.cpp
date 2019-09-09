#include "KFMongoConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMongoList::Reset()
    {
        _mongo_list.clear();
        _kf_seting = nullptr;
    }

    void KFMongoList::AddSetting( KFMongoSetting& kfsetting )
    {
        _mongo_list.push_back( kfsetting );
    }

    const KFMongoSetting* KFMongoList::FindSetting()
    {
        if ( _kf_seting == nullptr )
        {
            auto index = KFGlobal::Instance()->_app_id->GetId() % static_cast< uint32 >( _mongo_list.size() );
            _kf_seting = &_mongo_list[ index ];
        }

        return _kf_seting;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    KFMongoList* KFMongoType::FindMongoList( uint32 type )
    {
        return _mongo_list.Find( type );
    }

    KFMongoList* KFMongoType::AddMongoList( uint32 type )
    {
        return _mongo_list.Create( type );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    KFMongoType* KFMongoConfig::FindMongoType( const std::string& module, uint32 logicid )
    {
        auto key = ModuleKey( module, logicid );
        auto mongotype = _mongo_type.Find( key );
        if ( mongotype == nullptr )
        {
            mongotype = _mongo_type.First();
        }

        return mongotype;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMongoConfig::LoadConfig( const std::string& file, uint32 loadmask )
    {
        auto _mongo_id = 0u;
        _mongo_type.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( file );
        auto config = kfxml.RootNode();

        auto modulenode = config.FindNode( "Module" );
        while ( modulenode.IsValid() )
        {
            auto name = modulenode.GetString( "Name" );
            auto usessl = modulenode.GetString( "UseSSL" );
            auto authtype = modulenode.GetString( "Auth" );
            auto connecttimeout = modulenode.GetUInt32( "ConnectTimeout" );
            auto executetimeout = modulenode.GetUInt32( "ExecuteTimeout" );

            auto mongonode = modulenode.FindNode( "Mongo" );
            while ( mongonode.IsValid() )
            {
                auto minlogicid = mongonode.GetUInt32( "MinLogicId" );
                auto maxlogicid = mongonode.GetUInt32( "MaxLogicId" );

                for ( auto i = minlogicid; i <= maxlogicid; ++i )
                {
                    auto kfmongotype = _mongo_type.Create( ModuleKey( name, i ) );
                    kfmongotype->_id = ++_mongo_id;

                    // write
                    auto writenode = mongonode.FindNode( "Write" );
                    while ( writenode.IsValid() )
                    {
                        KFMongoSetting kfsetting;

                        kfsetting._use_ssl = usessl;
                        kfsetting._auth_type = authtype;
                        kfsetting._connect_timeout = connecttimeout;
                        kfsetting._execute_timeout = executetimeout;
                        kfsetting._ip = writenode.GetString( "IP" );
                        kfsetting._port = writenode.GetUInt32( "Port" );
                        kfsetting._database = writenode.GetString( "Database" );
                        kfsetting._user = writenode.GetString( "User" );
                        kfsetting._password = writenode.GetString( "Password" );

                        auto kfmongolist = kfmongotype->AddMongoList( KFDatabaseEnum::Write );
                        kfmongolist->AddSetting( kfsetting );

                        writenode.NextNode( "Write" );
                    }

                    // read
                    auto readnode = mongonode.FindNode( "Read" );
                    while ( readnode.IsValid() )
                    {
                        KFMongoSetting kfsetting;
                        kfsetting._use_ssl = usessl;
                        kfsetting._auth_type = authtype;
                        kfsetting._connect_timeout = connecttimeout;
                        kfsetting._execute_timeout = executetimeout;
                        kfsetting._ip = readnode.GetString( "IP" );
                        kfsetting._port = readnode.GetUInt32( "Port" );
                        kfsetting._database = readnode.GetString( "Database" );
                        kfsetting._user = readnode.GetString( "User" );
                        kfsetting._password = readnode.GetString( "Password" );

                        auto kfmongolist = kfmongotype->AddMongoList( KFDatabaseEnum::Read );
                        kfmongolist->AddSetting( kfsetting );

                        readnode.NextNode( "Read" );
                    }
                }

                mongonode.NextNode();
            }

            modulenode.NextNode();
        }

        return true;
    }
}
