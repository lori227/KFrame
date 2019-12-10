#include "KFRouteConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRouteConfig::ReadSetting( KFNode& xmlnode, KFRouteSetting* kfsetting )
    {
        kfsetting->_module = xmlnode.GetString( "Id" );
        kfsetting->_process = xmlnode.GetString( "Process" );
    }
}