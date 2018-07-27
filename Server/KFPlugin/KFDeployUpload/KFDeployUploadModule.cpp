#include "KFDeployUploadModule.h"
#include "KFJson.h"


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
        KFJson request( data );

        auto ftpid = request.GetUInt32( __KF_STRING__( ftpid ) );
        auto apppath = request.GetString( __KF_STRING__( apppath ) );

        // 启动ftp下载
        _kf_ftp->StartUpload( ftpid, apppath );

        return _kf_http_server->SendResponse( request );
    }

}