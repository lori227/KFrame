#include "KFDataShardConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDataShardConfig::LoadConfig( const std::string& file, uint32 loadmask )
    {
        _settings.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( file );
        auto config = kfxml.RootNode();
        auto savenode = config.FindNode( "Save" );

        auto databasenode = savenode.FindNode( "Database" );
        while ( databasenode.IsValid() )
        {
            auto name = databasenode.GetString( "Name" );
            auto kfsetting = _settings.Create( name );

            kfsetting->_id = name;
            kfsetting->_save_flag = databasenode.GetUInt32( "SaveFlag" );
            kfsetting->_sort = databasenode.GetUInt32( "Sort" );
            kfsetting->_is_open = databasenode.GetUInt32( "Open" );
            kfsetting->_cache_time = databasenode.GetUInt32( "CacheTime" );

            databasenode.NextNode();
        }
    }
}