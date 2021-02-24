#include "KFPayClientModule.hpp"

namespace KFrame
{
    void KFPayClientModule::BeforeRun()
    {
        __REGISTER_PLAYER_ENTER__( &KFPayClientModule::OnEnterPayModule );
        __REGISTER_PLAYER_LEAVE__( &KFPayClientModule::OnLeavePayModule );
        ////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_APPLY_PAY_ORDER_REQ, &KFPayClientModule::HandleApplyPayOrderReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_PAY_RESULT_REQ, &KFPayClientModule::HandlePayResultReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_QUERY_PAY_REQ, &KFPayClientModule::HandleQueryPayReq );
    }

    void KFPayClientModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_PLAYER_ENTER__();
        __UN_PLAYER_LEAVE__();
        //////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_APPLY_PAY_ORDER_REQ );
        __UN_MESSAGE__( KFMsg::MSG_PAY_RESULT_REQ );
        __UN_MESSAGE__( KFMsg::MSG_QUERY_PAY_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_PLAYER_ENTER_FUNCTION__( KFPayClientModule::OnEnterPayModule )
    {
        auto order = player->Get< std::string >( __STRING__( payorder ) );
        if ( order.empty() )
        {
            return;
        }

        QueryPayData( player->GetKeyID() );
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFPayClientModule::OnLeavePayModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFPayClientModule::HandleApplyPayOrderReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgApplyPayOrderReq );

        // 申请充值订单号
        auto setting = KFPayConfig::Instance()->FindSetting( kfmsg->payid() );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::PayIdError );
        }

        auto source = __FORMAT__( "{}-{}-{}", player_id, kfmsg->payid(), KFGlobal::Instance()->STMakeUuid() );
        auto order = KFCrypto::Md5Encode( source );

        // 发送给auth服务器
        static auto _url = _kf_ip_address->GetAuthUrl() + __STRING__( applyorder );

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __STRING__( payorder ), order );
        __JSON_SET_VALUE__( kfjson, __STRING__( playerid ), player_id );
        __JSON_SET_VALUE__( kfjson, __STRING__( payid ), setting->_id );
        __JSON_SET_VALUE__( kfjson, __STRING__( price ), setting->_price );
        _kf_http_client->MTGet( _url, kfjson, this, &KFPayClientModule::OnHttpApplyOrderCallBack );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFPayClientModule::OnHttpApplyOrderCallBack )
    {
        __JSON_PARSE_STRING__( kfjson, recvdata );

        auto player_id = __JSON_GET_UINT64__( kfjson, __STRING__( playerid ) );
        auto player = _kf_player->FindPlayer( player_id, __FUNC_LINE__ );
        if ( player == nullptr )
        {
            return;
        }

        auto retcode = __JSON_GET_UINT32__( kfjson, __STRING__( retcode ) );
        if ( retcode != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, retcode );
        }

        auto payid = __JSON_GET_STRING__( kfjson, __STRING__( payid ) );
        auto order = __JSON_GET_STRING__( kfjson, __STRING__( payorder ) );
        if ( payid.empty() || order.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::HttpDataError );
        }

        KFMsg::MsgApplyPayOrderAck ack;
        ack.set_payid( payid );
        ack.set_order( order );
        _kf_player->SendToClient( player, KFMsg::MSG_APPLY_PAY_ORDER_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFPayClientModule::HandlePayResultReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgPayResultReq );

        if ( kfmsg->result() )
        {
            // 启动定时器, 查询充值信息
            StartQueryPayTimer( player );
            player->UpdateData( __STRING__( payorder ), kfmsg->order() );

            __LOG_INFO__( "player=[{}] payid=[{}] order=[{}] pay ok", player_id, kfmsg->payid(), kfmsg->order() );
        }
        else
        {
            static auto _url = _kf_ip_address->GetAuthUrl() + __STRING__( removeorder );

            __JSON_OBJECT_DOCUMENT__( kfjson );
            __JSON_SET_VALUE__( kfjson, __STRING__( payorder ), kfmsg->order() );
            _kf_http_client->MTGet< KFPayClientModule >( _url, kfjson );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFPayClientModule::HandleQueryPayReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryPayReq );

        // 如果出现异常, 客户端可以通过这个消息来查询充值信息
        QueryPayData( player_id );
    }

    void KFPayClientModule::StartQueryPayTimer( EntityPtr player )
    {
        auto kfpayorder = player->Find( __STRING__( payorder ) );
        auto querytime = kfpayorder->_data_setting->_int_run_param;
        auto querycount = kfpayorder->_data_setting->_int_max_value;
        __COUNT_TIMER_1__( player->GetKeyID(), querytime, querycount, &KFPayClientModule::OnTimerQueryPayData );
    }

    __KF_TIMER_FUNCTION__( KFPayClientModule::OnTimerQueryPayData )
    {
        QueryPayData( objectid );
    }

    void KFPayClientModule::QueryPayData( uint64 player_id )
    {
        static auto _url = _kf_ip_address->GetAuthUrl() + __STRING__( querypay );

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __STRING__( playerid ), player_id );
        _kf_http_client->MTGet( _url, kfjson, this, &KFPayClientModule::OnHttpQueryPayCallBack );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFPayClientModule::OnHttpQueryPayCallBack )
    {
        __JSON_PARSE_STRING__( kfjson, recvdata );
        if ( !__JSON_HAS_MEMBER__( kfjson, __STRING__( paydata ) ) )
        {
            return;
        }

        auto player_id = __JSON_GET_UINT64__( kfjson, __STRING__( playerid ) );
        auto player = _kf_player->FindPlayer( player_id );
        if ( player == nullptr )
        {
            // 应该重置订单状态
            return __LOG_WARN__( "player=[{}] not online", player_id );
        }

        const auto& paydata = __JSON_GET_ARRRY__( kfjson, __STRING__( paydata ) );
        auto size = __JSON_ARRAY_SIZE__( paydata );
        for ( auto i = 0u; i < size; ++i )
        {
            auto& kfpay = __JSON_ARRAY_INDEX__( paydata, i );

            auto payid = __JSON_GET_STRING__( kfpay, __STRING__( payid ) );
            auto order = __JSON_GET_STRING__( kfpay, __STRING__( payorder ) );
            if ( payid.empty() || order.empty() )
            {
                __LOG_ERROR__( "player=[{}] payid=[{}] order=[{}] empty", player_id, payid, order );
                continue;
            }

            ProcessPay( player, payid, order );
        }

        // 停止定时器
        __UN_TIMER_1__( player_id );
    }

    void KFPayClientModule::ProcessPay( EntityPtr player, const std::string& payid, const std::string& order )
    {
        __LOG_INFO__( "player=[{}] payid=[{}] order=[{}] add element", player->GetKeyID(), payid, order );

        auto setting = KFPayConfig::Instance()->FindSetting( payid );
        if ( setting == nullptr )
        {
            return __LOG_ERROR__( "payid=[{}] can't find setting", payid );
        }

        // 判断是否首冲
        auto firstvalue = player->Get<uint64>( __STRING__( pay ), setting->_note_id, __STRING__( value ) );

        // 添加元素
        player->AddElement( &setting->_reward, _default_multiple, __STRING__( pay ), firstvalue, __FUNC_LINE__ );
        if ( firstvalue == _invalid_int )
        {
            // 添加元素
            player->AddElement( &setting->_first_reward, _default_multiple, __STRING__( firstpay ), firstvalue, __FUNC_LINE__ );
        }

        // 更新变量
        player->UpdateRecord( __STRING__( pay ), setting->_note_id, __STRING__( value ), KFEnum::Add, 1 );

        // 清空order
        auto playerorder = player->Get< std::string>( __STRING__( payorder ) );
        if ( playerorder == order )
        {
            player->UpdateData( __STRING__( payorder ), _invalid_string );
        }

        // 通知服务器, 更新充值状态
        static auto _url = _kf_ip_address->GetAuthUrl() + __STRING__( finishpay );

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __STRING__( payorder ), order );
        __JSON_SET_VALUE__( kfjson, __STRING__( playerid ), player->GetKeyID() );
        _kf_http_client->MTGet< KFPayClientModule >( _url, kfjson );
    }
}
