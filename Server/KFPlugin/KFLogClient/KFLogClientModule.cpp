#include "KFLogClientModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFLogClientModule::BeforeRun()
    {
        _kf_cluster->RegisterConnectionFunction( __KF_STRING__( log ), this, &KFLogClientModule::OnConnectionLogCluster );
    }

    void KFLogClientModule::BeforeShut()
    {
        _kf_cluster->UnRegisterConnectionFunction( __KF_STRING__( log ) );
    }

    void KFLogClientModule::OnConnectionLogCluster( uint32 serverid )
    {
        auto kfglobal = KFGlobal::Instance();

        // 注册log函数
        kfglobal->RegisterRemoteLogFunction( this, &KFLogClientModule::LogRemote );
    }

    bool KFLogClientModule::LogRemote( uint32 loglevel, const std::string& loginfo )
    {
        auto kfglobal = KFGlobal::Instance();

        KFMsg::S2SLogReq req;
        req.set_log_level( loglevel );
        req.set_app_name( kfglobal->_app_name );
        req.set_app_type( kfglobal->_app_type );
        req.set_app_id( kfglobal->_app_id );
        req.set_zone_id( kfglobal->_zone_id );
        req.set_log_info( loginfo );

        return _kf_cluster->SendMessageToShard( __KF_STRING__( log ), KFMsg::S2S_LOG_REQ, &req );
    }

}



