#include "KFCompoundModule.hpp"
#include "KFCompoundConfig.hpp"
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
        __REGISTER_MESSAGE__( KFMsg::MSG_COMPOUND_REQ, &KFCompoundModule::HandleCompoundReq );
    }

    void KFCompoundModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__( _kf_compound_config );
        ///////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_COMPOUND_REQ );
    }

    __KF_MESSAGE_FUNCTION__( KFCompoundModule::HandleCompoundReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgCompoundReq );

        auto kfobject = player->GetData();

        // 查找合成信息
        auto kfsetting = _kf_compound_config->FindCompoundSetting( kfmsg.dataname(), kfmsg.key() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::CompoundNotExist );
        }

        // 判断是否足够
        if ( player->CheckElement( &kfsetting->_cost_data, __FUNC_LINE__ ) )
        {
            return _kf_display->SendToClient( player, KFMsg::CompoundNotEnoughData );
        }

        // 扣除材料
        player->RemoveElement( &kfsetting->_cost_data, __FUNC_LINE__ );

        // 添加合成的属性
        player->AddElement( &kfsetting->_compound_data, true, __FUNC_LINE__ );
        _kf_display->SendToClient( player, KFMsg::CompoundOk );
    }
}