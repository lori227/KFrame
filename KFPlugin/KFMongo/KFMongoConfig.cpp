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
    bool KFMongoConfig::LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask )
    {
        auto _mongo_id = 0u;
        _mongo_type.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( filepath );
        auto config = kfxml.RootNode();

        auto modulenode = config.FindNode( "Module" );
        while ( modulenode.IsValid() )
        {
            auto name = modulenode.ReadString( "Name" );
            auto usessl = modulenode.ReadString( "UseSSL" );
            auto authtype = modulenode.ReadString( "Auth" );
            auto connecttimeout = modulenode.ReadUInt32( "ConnectTimeout" );
            auto executetimeout = modulenode.ReadUInt32( "ExecuteTimeout" );

            auto mongonode = modulenode.FindNode( "Mongo" );
            while ( mongonode.IsValid() )
            {
                auto minlogicid = mongonode.ReadUInt32( "MinLogicId" );
                auto maxlogicid = mongonode.ReadUInt32( "MaxLogicId" );

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
                        kfsetting._ip = writenode.ReadString( "IP" );
                        kfsetting._port = writenode.ReadUInt32( "Port" );
                        kfsetting._database = writenode.ReadString( "Database" );
                        kfsetting._user = writenode.ReadString( "User" );
                        kfsetting._password = writenode.ReadString( "Password" );

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
                        kfsetting._ip = readnode.ReadString( "IP" );
                        kfsetting._port = readnode.ReadUInt32( "Port" );
                        kfsetting._database = readnode.ReadString( "Database" );
                        kfsetting._user = readnode.ReadString( "User" );
                        kfsetting._password = readnode.ReadString( "Password" );

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
