#include "KFGroupShardConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFGroupShardConfig::KFGroupShardConfig()
    {
        _max_member_count = 4;
    }

    KFGroupShardConfig::~KFGroupShardConfig()
    {

    }

    bool KFGroupShardConfig::LoadConfig( const char* file )
    {
        try
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();

            //////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////
            {
                auto xmlnode = config.FindNode( "Group" );
                _max_member_count = xmlnode.GetUInt32( "MaxMemberCount" );
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }

}