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
            KFDirListType kftype;

            kftype._min_channel = dirlisttype.GetUInt32( "MinChannel" );
            kftype._max_channel = dirlisttype.GetUInt32( "MaxChannel" );
            kftype._list_type = dirlisttype.GetUInt32( "Type" );
            _dir_list_type.push_back( kftype );

            dirlisttype.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }

    uint32 KFDirConfig::GetDirListType( uint32 appflag )
    {
        for ( auto& kftype : _dir_list_type )
        {
            if ( appflag >= kftype._min_channel && appflag <= kftype._max_channel )
            {
                return kftype._list_type;
            }
        }

        return _default_dir_list_type;
    }
}