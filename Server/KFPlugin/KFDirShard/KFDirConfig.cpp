#include "KFDirConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFDirConfig::KFDirConfig()
    {
        _default_dir_list_type = __TOTAL_SERVER_LIST__;
    }

    bool KFDirConfig::LoadConfig()
    {
        _dir_list_type.clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto dirlisttypes = config.FindNode( "DirListTypes" );
        _default_dir_list_type = dirlisttypes.GetUInt32( "Default" );

        auto dirlisttype = dirlisttypes.FindNode( "DirListType" );
        while ( dirlisttype.IsValid() )
        {
            auto channel = dirlisttype.GetUInt32( "Channel" );
            auto type = dirlisttype.GetUInt32( "Type" );
            _dir_list_type[ channel ] = type;

            dirlisttype.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }

    uint32 KFDirConfig::GetDirListType( uint32 appchannel )
    {
        auto iter = _dir_list_type.find( appchannel );
        if ( iter == _dir_list_type.end() )
        {
            return _default_dir_list_type;
        }

        return iter->second;
    }
}