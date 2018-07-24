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
        __REGISTER_HTTP_FUNCTION__( KFField::_upload, true, &KFDeployUploadModule::HandleUploadFile );
    }


    void KFDeployUploadModule::ShutDown()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_HTTP_FUNCTION__( KFField::_upload );
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFDeployUploadModule::HandleUploadFile )
    {
        KFJson request( data );

        auto ftpid = request.GetUInt32( KFField::_ftp_id );
        auto apppath = request.GetString( KFField::_app_path );

        // 启动ftp下载
        _kf_ftp->StartUpload( ftpid, apppath );

        return _kf_http_server->SendResponse( request );
    }

}