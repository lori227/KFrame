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
        auto arraynode = config.FindNode( "ArrayIndex" );
        kfglobal->_array_index = arraynode.GetUInt32( "Value" );
        //////////////////////////////////////////////////////////////////
        auto disconnetnode = config.FindNode( "Disconnet" );
        auto disconnecttime = disconnetnode.GetUInt32( "Time" );
        kfglobal->AddConstant( __STRING__( disconnecttime ), 0u, disconnecttime );
        //////////////////////////////////////////////////////////////////
        auto rewardnode = config.FindNode( "RewardType" );
        auto rewardtype = rewardnode.GetUInt32( "Value" );
        kfglobal->AddConstant( __STRING__( rewardtype ), 0u, rewardtype );
        //////////////////////////////////////////////////////////////////
        auto databasenodes = config.FindNode( "Databases" );
        if ( databasenodes.IsValid() )
        {
            auto databasenode = databasenodes.FindNode( "Database" );
            while ( databasenode.IsValid() )
            {
                auto name = databasenode.GetString( "Name" );
                auto type = databasenode.GetUInt32( "Type" );
                kfglobal->AddConstant( name, 0u, type );

                databasenode.NextNode();
            }
        }
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