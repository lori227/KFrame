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
        _compress_type = savenode.GetUInt32( "CompressType" );
        _compress_level = savenode.GetUInt32( "CompressLevel" );


        auto databasenode = savenode.FindNode( "Database" );
        while ( databasenode.IsValid() )
        {
            auto type = databasenode.GetUInt32( "Type" );
            auto kfsetting = _settings.Create( type );

            kfsetting->_id = type;
            kfsetting->_save_flag = databasenode.GetUInt32( "SaveFlag" );
            kfsetting->_sort = databasenode.GetUInt32( "Sort" );
            kfsetting->_is_open = databasenode.GetUInt32( "Open" );
            kfsetting->_cache_time = databasenode.GetUInt32( "CacheTime" );


            databasenode.NextNode();
        }

        return true;
    }
}