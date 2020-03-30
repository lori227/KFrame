#include "KFProjectConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFProjectConfig::LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask )
    {
        auto kfglobal = KFGlobal::Instance();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( filepath );
        auto config = kfxml.RootNode();
        //////////////////////////////////////////////////////////////////
        auto projectnode = config.FindNode( "Project" );
        kfglobal->_project_time = projectnode.GetUInt64( "Time" );
        //////////////////////////////////////////////////////////////////
        auto disconnetnode = config.FindNode( "Disconnet" );
        auto disconnecttime = disconnetnode.GetUInt32( "Time" );
        kfglobal->AddConstant( __STRING__( disconnecttime ), 0u, disconnecttime );
        //////////////////////////////////////////////////////////////////
        auto databasenode = config.FindNode( "Database" );

        auto authdatabase = databasenode.GetUInt32( "Auth" );
        kfglobal->AddConstant( __STRING__( authdatabase ), 0u, authdatabase );

        auto dirdatabase = databasenode.GetUInt32( "Dir" );
        kfglobal->AddConstant( __STRING__( dirdatabase ), 0u, dirdatabase );

        auto paydatabse = databasenode.GetUInt32( "Pay" );
        kfglobal->AddConstant( __STRING__( paydatabase ), 0u, paydatabse );
        //////////////////////////////////////////////////////////////////
        auto uuidnodes = config.FindNode( "Uuids" );
        if ( uuidnodes.IsValid() )
        {
            auto uuidnode = uuidnodes.FindNode( "Uuid" );
            while ( uuidnode.IsValid() )
            {
                auto name = uuidnode.GetString( "Name" );
                auto timebits = uuidnode.GetUInt32( "Time" );
                auto zonebits = uuidnode.GetUInt32( "Zone" );
                auto workerbits = uuidnode.GetUInt32( "Worker" );
                auto seqbits = uuidnode.GetUInt32( "Seq" );
                kfglobal->AddUuidSetting( name, timebits, zonebits, workerbits, seqbits );

                uuidnode.NextNode();
            }
        }
        //////////////////////////////////////////////////////////////////
        return true;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFIntSetting* KFConstantConfig::CreateSetting( KFNode& xmlnode )
    {
        auto service = xmlnode.GetUInt32( "Service", true );
        auto channel = xmlnode.GetUInt32( "Channel", true );
        auto ok = KFGlobal::Instance()->CheckChannelService( channel, service );
        if ( !ok )
        {
            return nullptr;
        }

        return _settings.Create( 0u );
    }

    void KFConstantConfig::ReadSetting( KFNode& xmlnode, KFIntSetting* kfsetting )
    {
        auto name = xmlnode.GetString( "Name" );
        auto key = xmlnode.GetUInt32( "Key", true );
        auto value = xmlnode.GetString( "Value" );
        KFGlobal::Instance()->AddConstant( name, key, value );
    }
}