#include "KFDirProxyModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFDirProxyModule::KFDirProxyModule()
    {
    }

    KFDirProxyModule::~KFDirProxyModule()
    {
    }

    void KFDirProxyModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST_FUNCTION__( &KFDirProxyModule::OnServerLostClient );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_ONLINE_TO_DIR_REQ, &KFDirProxyModule::HandleUpdateOnlineToDirReq );
    }

    void KFDirProxyModule::BeforeShut()
    {
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_ONLINE_TO_DIR_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFDirProxyModule::HandleUpdateOnlineToDirReq )
    {
        // 广播给所有的shard
        _kf_tcp_client->SendMessageToType( __KF_STRING__( shard ), KFMsg::S2S_UPDATE_ONLINE_TO_DIR_REQ, data, length );
    }

    __KF_SERVER_LOST_FUNCTION__( KFDirProxyModule::OnServerLostClient )
    {
        if ( handlename != __KF_STRING__( zone ) || handletype != __KF_STRING__( gate ) )
        {
            return;
        }

        KFMsg::S2SRemoveOnlineToDirReq req;
        req.set_appid( handleid );
        _kf_tcp_client->SendMessageToType( __KF_STRING__( shard ), KFMsg::S2S_REMOVE_ONLINE_TO_DIR_REQ, &req );
    }
}