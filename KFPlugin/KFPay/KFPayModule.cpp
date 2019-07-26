#include "KFPayModule.hpp"

namespace KFrame
{
    void KFPayModule::InitModule()
    {
        __KF_ADD_CONFIG__( KFPayConfig );
    }

    void KFPayModule::BeforeRun()
    {
        __REGISTER_ENTER_PLAYER__( &KFPayModule::OnEnterPayModule );
        __REGISTER_LEAVE_PLAYER__( &KFPayModule::OnLeavePayModule );
        ////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_APPLY_PAY_ORDER_REQ, &KFPayModule::HandleApplyPayOrderReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_PAY_RESULT_REQ, &KFPayModule::HandlePayResultReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_PAY_REQ, &KFPayModule::HandleQueryPayReq );
    }

    void KFPayModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        //////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_APPLY_PAY_ORDER_REQ );
        __UN_MESSAGE__( KFMsg::MSG_PAY_RESULT_REQ );
        __UN_MESSAGE__( KFMsg::MSG_QUERY_PAY_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ENTER_PLAYER_FUNCTION__( KFPayModule::OnEnterPayModule )
    {
        auto kfobject = player->GetData();
        auto order = kfobject->GetValue< std::string >( __KF_STRING__( payorder ) );
        if ( order.empty() )
        {
            return;
        }

        QueryPayData( player->GetKeyID() );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFPayModule::OnLeavePayModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFPayModule::HandleApplyPayOrderReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgApplyPayOrderReq );

        // 申请充值订单号
        auto kfsetting = KFPayConfig::Instance()->FindSetting( kfmsg.payid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::PayIdError );
        }

        auto source = __FORMAT__( "{}-{}-{}", playerid, kfmsg.payid(), KFGlobal::Instance()->MakeUUID() );
        auto order = KFCrypto::Md5Encode( source );

        // 发送给auth服务器
        static auto _url = _kf_ip_address->GetAuthUrl() + __KF_STRING__( applyorder );

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( payorder ), order );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( playerid ), playerid );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( payid ), kfsetting->_id );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( price ), kfsetting->_price );
        _kf_http_client->MTGet( _url, kfjson, this, &KFPayModule::OnHttpApplyOrderCallBack );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFPayModule::OnHttpApplyOrderCallBack )
    {
        __JSON_PARSE_STRING__( kfjson, recvdata );

        auto playerid = __JSON_GET_UINT64__( kfjson, __KF_STRING__( playerid ) );
        auto player = _kf_player->FindPlayer( playerid, __FUNC_LINE__ );
        if ( player == nullptr )
        {
            return;
        }

        auto retcode = __JSON_GET_UINT32__( kfjson, __KF_STRING__( retcode ) );
        if ( retcode != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, retcode );
        }

        auto payid = __JSON_GET_STRING__( kfjson, __KF_STRING__( payid ) );
        auto order = __JSON_GET_STRING__( kfjson, __KF_STRING__( payorder ) );
        if ( payid.empty() || order.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::HttpDataError );
        }

        KFMsg::MsgApplyPayOrderAck ack;
        ack.set_payid( payid );
        ack.set_order( order );
        _kf_player->SendToClient( player, KFMsg::MSG_APPLY_PAY_ORDER_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFPayModule::HandlePayResultReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgPayResultReq );

        if ( kfmsg.result() )
        {
            // 启动定时器, 查询充值信息
            StartQueryPayTimer( player );
            player->UpdateData( __KF_STRING__( payorder ), kfmsg.order() );

            __LOG_INFO__( "player=[{}] payid=[{}] order=[{}] pay ok!", playerid, kfmsg.payid(), kfmsg.order() );
        }
        else
        {
            static auto _url = _kf_ip_address->GetAuthUrl() + __KF_STRING__( removeorder );

            __JSON_OBJECT_DOCUMENT__( kfjson );
            __JSON_SET_VALUE__( kfjson, __KF_STRING__( payorder ), kfmsg.order() );
            _kf_http_client->MTGet< KFPayModule >( _url, kfjson );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFPayModule::HandleQueryPayReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryPayReq );

        // 如果出现异常, 客户端可以通过这个消息来查询充值信息
        QueryPayData( playerid );
    }

    void KFPayModule::StartQueryPayTimer( KFEntity* player )
    {
        auto kfpayorder = player->GetData()->FindData( __KF_STRING__( payorder ) );
        auto querytime = kfpayorder->_data_setting->_int_logic_value;
        auto querycount = kfpayorder->_data_setting->_int_max_value;
        __LIMIT_TIMER_1__( player->GetKeyID(), querytime, querycount, &KFPayModule::OnTimerQueryPayData );
    }

    __KF_TIMER_FUNCTION__( KFPayModule::OnTimerQueryPayData )
    {
        QueryPayData( objectid );
    }

    void KFPayModule::QueryPayData( uint64 playerid )
    {
        static auto _url = _kf_ip_address->GetAuthUrl() + __KF_STRING__( querypay );

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( playerid ), playerid );
        _kf_http_client->MTGet< KFPayModule >( _url, kfjson, this, &KFPayModule::OnHttpQueryPayCallBack );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFPayModule::OnHttpQueryPayCallBack )
    {
        __JSON_PARSE_STRING__( kfjson, recvdata );
        if ( !__JSON_HAS_MEMBER__( kfjson, __KF_STRING__( paydata ) ) )
        {
            return;
        }

        auto playerid = __JSON_GET_UINT64__( kfjson, __KF_STRING__( playerid ) );
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            // 应该重置订单状态
            return __LOG_WARN__( "player=[{}] not online!", playerid );
        }

        const auto& paydata = __JSON_GET_ARRRY__( kfjson, __KF_STRING__( paydata ) );
        auto size = __JSON_ARRAY_SIZE__( paydata );
        for ( auto i = 0u; i < size; ++i )
        {
            auto& kfpay = __JSON_ARRAY_INDEX__( paydata, i );

            auto payid = __JSON_GET_STRING__( kfpay, __KF_STRING__( payid ) );
            auto order = __JSON_GET_STRING__( kfpay, __KF_STRING__( payorder ) );
            if ( payid.empty() || order.empty() )
            {
                __LOG_ERROR__( "player=[{}] payid=[{}] order=[{}] empty!", playerid, payid, order );
                continue;
            }

            ProcessPay( player, payid, order );
        }

        // 停止定时器
        __UN_TIMER_1__( playerid );
    }

    void KFPayModule::ProcessPay( KFEntity* player, const std::string& payid, const std::string& order )
    {
        __LOG_INFO__( "player=[{}] payid=[{}] order=[{}] add element!", player->GetKeyID(), payid, order );

        auto kfsetting = KFPayConfig::Instance()->FindSetting( payid );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "payid=[{}] can't find setting!", payid );
        }

        // 添加元素
        player->AddElement( &kfsetting->_buy_elements, false, __FUNC_LINE__ );

        // 判断是否首冲
        auto kfobject = player->GetData();
        auto firstvalue = kfobject->GetValue< uint64 >( __KF_STRING__( variable ), kfsetting->_first_variable_id, __KF_STRING__( value ) );
        if ( firstvalue == _invalid_int )
        {
            // 更新变量
            player->UpdateData( __KF_STRING__( variable ), kfsetting->_first_variable_id, __KF_STRING__( value ), KFEnum::Set, 1 );

            // 添加元素
            player->AddElement( &kfsetting->_first_elements, false, __FUNC_LINE__ );
        }

        // 清空order
        auto playerorder = kfobject->GetValue< std::string>( __KF_STRING__( payorder ) );
        if ( playerorder == order )
        {
            player->UpdateData( __KF_STRING__( payorder ), _invalid_str );
        }

        // 通知服务器, 更新充值状态
        static auto _url = _kf_ip_address->GetAuthUrl() + __KF_STRING__( finishpay );

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( payorder ), order );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( playerid ), player->GetKeyID() );
        _kf_http_client->MTGet< KFPayModule >( _url, kfjson );
    }
}
