#include "KFDataShardConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDataShardConfig::LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask )
    {
        _settings.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( filepath );
        auto config = kfxml.RootNode();
        auto savenode = config.FindNode( "Save" );
        _compress_type = savenode.ReadUInt32( "CompressType" );
        _compress_level = savenode.ReadUInt32( "CompressLevel" );


        auto databasenode = savenode.FindNode( "Database" );
        while ( databasenode.IsValid() )
        {
            auto type = databasenode.ReadUInt32( "Type" );
            auto kfsetting = _settings.Create( type );

            kfsetting->_id = type;
            kfsetting->_save_flag = databasenode.ReadUInt32( "SaveFlag" );
            kfsetting->_sort = databasenode.ReadUInt32( "Sort" );
            kfsetting->_is_open = databasenode.ReadUInt32( "Open" );
            kfsetting->_cache_time = databasenode.ReadUInt32( "CacheTime" );

            databasenode.NextNode();
        }

        return true;
    }
}