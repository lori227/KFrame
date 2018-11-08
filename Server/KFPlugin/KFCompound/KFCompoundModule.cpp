#include "KFCompoundModule.h"
#include "KFCompoundConfig.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFCompoundModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_compound_config, true );
        ///////////////////////////////////////////////////////////////////////////////
    }

    void KFCompoundModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::MSG_COMPOUND_DATA_REQ, &KFCompoundModule::HandleCompoundDataReq );
    }

    void KFCompoundModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__( _kf_compound_config );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_COMPOUND_DATA_REQ );
    }

    __KF_MESSAGE_FUNCTION__( KFCompoundModule::HandleCompoundDataReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgCompoundDataReq );

        auto kfobject = player->GetData();

        // 查找合成信息
        auto kfsetting = _kf_compound_config->FindCompoundSetting( kfmsg.dataname(), kfmsg.key() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::CompoundNotExist );
        }

        // 判断是否足够
        if ( player->CheckAgentData( &kfsetting->_cost_data, __FUNC_LINE__ ) )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough );
        }

        // 扣除材料
        player->RemoveAgentData( &kfsetting->_cost_data, __FUNC_LINE__ );

        // 添加合成的属性
        player->AddAgentData( &kfsetting->_compound_data, 1.0f, true, __FUNC_LINE__ );
    }
}