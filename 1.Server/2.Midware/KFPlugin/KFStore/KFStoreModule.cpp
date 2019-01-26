#include "KFStoreModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFStoreModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_store_config, true );
    }

    void KFStoreModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::MSG_BUY_STORE_REQ, &KFStoreModule::HandleBuyStoreReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_STORE_REQ, &KFStoreModule::HandleQueryStoreReq );
    }

    void KFStoreModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__( _kf_store_config );
        //////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_BUY_STORE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_STORE_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFStoreModule::HandleQueryStoreReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryStoreReq );

        // 版本号一直就不更新了
        if ( kfmsg.version() == _kf_store_config->_version )
        {
            return;
        }

        KFMsg::MsgQueryStoreAck ack;
        ack.set_version( _kf_store_config->_version );
        ack.set_data( _kf_store_config->_store_xml->Data(), _kf_store_config->_store_xml->Size() );
        _kf_player->SendToClient( player, KFMsg::MSG_QUERY_STORE_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFStoreModule::HandleBuyStoreReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgBuyStoreReq );

        auto result = ProcessBuyStore( player, kfmsg.buytype(), kfmsg.id(), kfmsg.count() );
        _kf_display->SendToClient( player, result, kfmsg.id() );
    }

    uint32 KFStoreModule::ProcessBuyStore( KFEntity* player, const std::string& buytype, uint32 id, uint32 count )
    {
        auto kfsetting = _kf_store_config->FindStoreSetting( id );
        if ( kfsetting == nullptr )
        {
            return KFMsg::StoreNotFind;
        }

        if ( count == 0 || count > kfsetting->_buy_max_count )
        {
            return KFMsg::StoreBuyCountError;
        }

        // 如果有设置拥有限制( 英雄, 皮肤等 ), 则不让购买
        if ( CheckOwnLimit( player, kfsetting ) )
        {
            return KFMsg::StoreOutOfLimitOwm;
        }

        // 判断是否限购
        if ( CheckBuyLimit( player, kfsetting, count ) )
        {
            return KFMsg::StoreOutOfLimits;
        }

        // 获得商品价格
        auto kfagents = kfsetting->FindCostElements( buytype, KFGlobal::Instance()->_real_time );
        if ( kfagents == nullptr )
        {
            return KFMsg::StoreBuyTypeError;
        }

        // 检测扣除物品是否足够
        if ( !player->CheckElement( __FUNC_LINE__, kfagents, count ) )
        {
            return KFMsg::StoreLackCost;
        }

        // 扣除道具
        player->RemoveElement( __FUNC_LINE__, kfagents, count );

        // 发送道具
        player->AddElement( __FUNC_LINE__, &kfsetting->_buy_elements, true, count );

        // 记录购买次数
        SaveBuyLimit( player, kfsetting, count );
        return KFMsg::StoreBuyOK;
    }

    bool KFStoreModule::CheckOwnLimit( KFEntity* player, const KFStoreSetting* kfsetting )
    {
        // 一般情况下, 只有item, hero, skin 等有这个需求
        if ( kfsetting->_max_own_count == 0 )
        {
            return false;
        }

        // 只判断是否存在, 不判断数量
        return player->CheckElement( __FUNC_LINE__, &kfsetting->_buy_elements );
    }

    bool KFStoreModule::CheckBuyLimit( KFEntity* player, const KFStoreSetting* kfsetting, uint32 count )
    {
        // 不限制
        if ( kfsetting->_buy_limit_type.empty() )
        {
            return false;
        }

        auto kfobject = player->GetData();
        auto kfstore = kfobject->FindData( kfsetting->_buy_limit_type, kfsetting->_id );
        if ( kfstore == nullptr )
        {
            return false;
        }

        // 比较购买次数
        auto datacount = kfstore->GetValue( __KF_STRING__( count ) );
        if ( ( datacount + count ) <= kfsetting->_buy_limit_count )
        {
            return false;
        }

        return true;
    }

    void KFStoreModule::SaveBuyLimit( KFEntity* player, const KFStoreSetting* kfsetting, uint32 count )
    {
        // 不限制
        if ( kfsetting->_buy_limit_type.empty() )
        {
            return;
        }

        // 更新数量
        player->UpdateData( kfsetting->_buy_limit_type, kfsetting->_id, __KF_STRING__( count ), KFOperateEnum::Add, count );
    }
}
