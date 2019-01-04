#include "KFDeployUploadModule.h"

namespace KFrame
{
    KFDeployUploadModule::KFDeployUploadModule()
    {
    }

    KFDeployUploadModule::~KFDeployUploadModule()
    {

    }

    void KFDeployUploadModule::BeforeRun()
    {

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( upload ), true, &KFDeployUploadModule::HandleUploadFile );
    }


    void KFDeployUploadModule::ShutDown()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( upload ) );
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFDeployUploadModule::HandleUploadFile )
    {
        __JSON_PARSE_STRING__( request, data );

        auto ftpid = __JSON_GET_UINT32__( request, __KF_STRING__( ftpid ) );
        auto apppath = __JSON_GET_STRING__( request, __KF_STRING__( apppath ) );

        // 启动ftp下载
        _kf_ftp->StartUpload( ftpid, apppath );

        return _kf_http_server->SendResponse( request );
    }

}