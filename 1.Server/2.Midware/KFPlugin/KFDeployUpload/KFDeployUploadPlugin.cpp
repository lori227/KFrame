#include "KFDeployUploadPlugin.h"
#include "KFDeployUploadModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDeployUploadPlugin::Install()
    {
        __REGISTER_MODULE__( KFDeployUpload );
    }

    void KFDeployUploadPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFDeployUpload );
    }

    void KFDeployUploadPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_ftp, KFFtpInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_http_server, KFHttpServerInterface );
    }
}