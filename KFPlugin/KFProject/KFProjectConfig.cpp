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
        kfglobal->_project_time = projectnode.ReadUInt64( "Time" );
        //////////////////////////////////////////////////////////////////
        auto arraynode = config.FindNode( "ArrayIndex" );
        kfglobal->_array_index = arraynode.ReadUInt32( "Value" );
        if ( kfglobal->_array_index > 1u )
        {
            kfglobal->_array_index = 1u;
        }
        //////////////////////////////////////////////////////////////////
        auto disconnetnode = config.FindNode( "Disconnet" );
        auto disconnecttime = disconnetnode.ReadUInt32( "Time" );
        kfglobal->AddConstant( __STRING__( disconnecttime ), 0u, disconnecttime );
        //////////////////////////////////////////////////////////////////
        auto rewardnode = config.FindNode( "RewardType" );
        auto rewardtype = rewardnode.ReadUInt32( "Value" );
        kfglobal->AddConstant( __STRING__( rewardtype ), 0u, rewardtype );
        //////////////////////////////////////////////////////////////////
        auto databasenodes = config.FindNode( "Databases" );
        if ( databasenodes.IsValid() )
        {
            auto databasenode = databasenodes.FindNode( "Database" );
            while ( databasenode.IsValid() )
            {
                auto name = databasenode.ReadString( "Name" );
                auto type = databasenode.ReadUInt32( "Type" );
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
                auto name = uuidnode.ReadString( "Name" );
                auto timebits = uuidnode.ReadUInt32( "Time" );
                auto zonebits = uuidnode.ReadUInt32( "Zone" );
                auto workerbits = uuidnode.ReadUInt32( "Worker" );
                auto seqbits = uuidnode.ReadUInt32( "Seq" );
                kfglobal->AddUuidSetting( name, timebits, zonebits, workerbits, seqbits );

                uuidnode.NextNode();
            }
        }
        //////////////////////////////////////////////////////////////////
        return true;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFIntSetting* KFConstantConfig::CreateSetting( KFXmlNode& xmlnode )
    {
        auto service = xmlnode.ReadUInt32( "Service", true );
        auto channel = xmlnode.ReadUInt32( "Channel", true );
        auto ok = KFGlobal::Instance()->CheckChannelService( channel, service );
        if ( !ok )
        {
            return nullptr;
        }

        return _settings.Create( 0u );
    }

    void KFConstantConfig::ReadSetting( KFXmlNode& xmlnode, KFIntSetting* kfsetting )
    {
        auto name = xmlnode.ReadString( "Name" );
        auto key = xmlnode.ReadUInt32( "Key", true );
        auto value = xmlnode.ReadString( "Value" );
        KFGlobal::Instance()->AddConstant( name, key, value );
    }
}