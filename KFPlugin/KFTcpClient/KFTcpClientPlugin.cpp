#include "KFTcpClientPlugin.hpp"
#include "KFTcpClientModule.hpp"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFTcpClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFTcpClient );
    }

    void KFTcpClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFTcpClient );
    }

    void KFTcpClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
    }
}