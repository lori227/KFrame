#include "KFBasicClientModule.hpp"

namespace KFrame
{
#define __ROUTE_NAME__ __STRING__( basic )

    void KFBasicClientModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_UPDATE_DATA__( &KFBasicClientModule::OnUpdateDataCallBack );
        __REGISTER_UPDATE_STRING__( &KFBasicClientModule::OnUpdateStringCallBack );

        __REGISTER_PLAYER_AFTER_ENTER__( &KFBasicClientModule::OnEnterUpdateBasicAttribute );
        __REGISTER_PLAYER_LEAVE__( &KFBasicClientModule::OnLeaveUpdateBasicAttribute );

        _kf_route->RegisterConnectionFunction( this, &KFBasicClientModule::OnRouteConnectCluster );
        ///////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_SET_SEX_REQ, &KFBasicClientModule::HandleSetSexReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_SET_NAME_REQ, &KFBasicClientModule::HandleSetNameReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_SET_PLAYER_NAME_TO_GAME_ACK, &KFBasicClientModule::HandleSetPlayerNameToGameAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_QUERY_BASIC_REQ,&KFBasicClientModule::HandleQueryBasicReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_QUERY_ATTRIBUTE_TO_GAME_ACK, &KFBasicClientModule::HandleQueryAttributeToGameAck );
    }

    void KFBasicClientModule::BeforeShut()
    {
        __UN_UPDATE_DATA__();
        __UN_UPDATE_STRING__();
        __UN_PLAYER_AFTER_ENTER__();
        __UN_PLAYER_LEAVE__();

        _kf_route->UnRegisterConnectionFunction( this );
        ///////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_SET_NAME_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SET_SEX_REQ );
        __UN_MESSAGE__( KFMsg::S2S_SET_PLAYER_NAME_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::MSG_QUERY_BASIC_REQ );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_ATTRIBUTE_TO_GAME_ACK );
    }

    void KFBasicClientModule::OnRouteConnectCluster( uint64 server_id )
    {
        if ( !_need_refresh_online )
        {
            return;
        }

        _need_refresh_online = false;

        // 清空该服务器上的玩家基础属性(server_id)
        KFMsg::S2SClearOnlineToBasicReq req;
        req.set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        _kf_route->RepeatToRand( __ROUTE_NAME__, KFMsg::S2S_CLEAR_ONLINE_TO_BASIC_REQ, &req );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFBasicClientModule::UpdateBasicIntValueToBasic( uint64 player_id, StringUInt64& values )
    {
        KFMsg::S2SUpdateIntValueToBasicReq req;
        req.mutable_pbdata()->insert( values.begin(), values.end() );
        _kf_route->RepeatToRand( player_id, __ROUTE_NAME__, KFMsg::S2S_UPDATE_INT_VALUE_TO_BASIC_REQ, &req );
    }

    void KFBasicClientModule::UpdateBasicIntValueToBasic( uint64 player_id, const std::string& data_name, uint64 data_value )
    {
        StringUInt64 values;
        values[ data_name ] = data_value;
        UpdateBasicIntValueToBasic( player_id, values );
    }

    void KFBasicClientModule::UpdateBasicStrValueToBasic( uint64 player_id, const std::string& data_name, const std::string& data_value )
    {
        KFMsg::S2SUpdateStrValueToBasicReq req;
        ( *req.mutable_pbdata() )[ data_name ] = data_value;
        _kf_route->RepeatToRand( player_id, __ROUTE_NAME__, KFMsg::S2S_UPDATE_STR_VALUE_TO_BASIC_REQ, &req );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_PLAYER_ENTER_FUNCTION__( KFBasicClientModule::OnEnterUpdateBasicAttribute )
    {
        EnterLeaveUpdateAttributeToBasic( player );
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFBasicClientModule::OnLeaveUpdateBasicAttribute )
    {
        EnterLeaveUpdateAttributeToBasic( player );
    }

    void KFBasicClientModule::EnterLeaveUpdateAttributeToBasic( EntityPtr player )
    {
        auto kfbasic = player->Find( __STRING__( basic ) );

        StringUInt64 values;
        values[ __STRING__( id ) ] = player->GetKeyID();
        values[ __STRING__( server_id ) ] = kfbasic->Get( __STRING__( server_id ) );
        values[ __STRING__( status ) ] = kfbasic->Get( __STRING__( status ) );
        values[ __STRING__( statustime ) ] = kfbasic->Get( __STRING__( statustime ) );
        UpdateBasicIntValueToBasic( player->GetKeyID(), values );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_UPDATE_DATA_FUNCTION__( KFBasicClientModule::OnUpdateDataCallBack )
    {
        // 只有公共属性才更新
        if ( !player->IsInited() ||
                !kfdata->HaveMask( KFDataDefine::DataMaskBasic ) ||
                !kfdata->GetParent()->HaveMask( KFDataDefine::DataMaskBasic ) )
        {
            return;
        }

        UpdateBasicIntValueToBasic( player->GetKeyID(), kfdata->_data_setting->_name, newvalue );
    }

    __KF_UPDATE_STRING_FUNCTION__( KFBasicClientModule::OnUpdateStringCallBack )
    {
        // 只有公共属性才更新
        if ( !player->IsInited() ||
                !kfdata->HaveMask( KFDataDefine::DataMaskBasic ) ||
                !kfdata->GetParent()->HaveMask( KFDataDefine::DataMaskBasic ) )
        {
            return;
        }

        UpdateBasicStrValueToBasic( player->GetKeyID(), kfdata->_data_setting->_name, newvalue );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    __KF_MESSAGE_FUNCTION__( KFBasicClientModule::HandleQueryBasicReq, KFMsg::MsgQueryBasicReq )
    {
        auto setting = KFZoneConfig::Instance()->FindSetting( KFGlobal::Instance()->_app_id->GetZoneId() );
        if ( setting == nullptr )
        {
            return __LOG_ERROR__( "can't find zone=[{}] setting", KFGlobal::Instance()->_app_id->GetZoneId() );
        }

        // 屏蔽字检查
        auto filter = _kf_filter->CheckFilter( kfmsg->name() );
        if ( filter )
        {
            return _kf_display->SendToClient( entity, KFMsg::NameFilterError );
        }

        // 发送到basic
        KFMsg::S2SQueryAttributeToBasicReq req;
        req.set_name( kfmsg->name() );
        req.set_zoneid( setting->_data_id );
        _kf_route->SendToRand( entity->GetKeyID(), __ROUTE_NAME__, KFMsg::S2S_QUERY_ATTRIBUTE_TO_BASIC_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFBasicClientModule::HandleQueryAttributeToGameAck, KFMsg::S2SQueryAttributeToGameAck )
    {
        if ( kfmsg->result() != KFMsg::Ok )
        {
            return _kf_display->SendToClient( entity, kfmsg->result(), kfmsg->name() );
        }

        auto kfbasic = entity->CreateData( __STRING__( basic ) );
        for ( auto iter = kfmsg->pbdata().begin(); iter != kfmsg->pbdata().end(); ++iter )
        {
            auto kfdata = kfbasic->Find( iter->first );
            if ( kfdata != nullptr )
            {
                kfdata->Set< std::string >( iter->second );
            }
        }
        auto player_data = _kf_kernel->SerializeToView( kfbasic );

        // 发送给客户端
        KFMsg::MsgQueryBasicAck ack;
        ack.mutable_player()->CopyFrom( *player_data );
        _kf_player->SendToClient( entity, KFMsg::MSG_QUERY_BASIC_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFBasicClientModule::HandleSetSexReq, KFMsg::MsgSetSexReq )
    {
        entity->UpdateObject( __STRING__( basic ), __STRING__( sex ), KFEnum::Set, kfmsg->sex() );
        //_kf_display->SendToClient( entity, KFMsg::SexSetOK );
    }

    uint32 KFBasicClientModule::CheckNameValid( const std::string& name, uint32 maxlength )
    {
        if ( name.size() > maxlength )
        {
            return KFMsg::NameLengthError;
        }

        auto ok = _kf_filter->CheckFilter( name );
        if ( ok )
        {
            return KFMsg::NameFilterError;
        }

        return KFMsg::Ok;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFBasicClientModule::HandleSetNameReq, KFMsg::MsgSetNameReq )
    {
        if ( kfmsg->name().empty() )
        {
            return _kf_display->SendToClient( entity, KFMsg::NameEmpty );
        }

        auto kfname = entity->Find( __STRING__( basic ), __STRING__( name ) );
        auto name = kfname->Get<std::string>();
        if ( !name.empty() )
        {
            return _kf_display->SendToClient( entity, KFMsg::NameAlreadySet );
        }

        // 检查名字的有效性
        auto result = CheckNameValid( kfmsg->name(), kfname->_data_setting->_int_max_value );
        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( entity, result );
        }

        auto setting = KFZoneConfig::Instance()->FindSetting( KFGlobal::Instance()->_app_id->GetZoneId() );
        if ( setting == nullptr )
        {
            return __LOG_ERROR__( "can't find zone=[{}] setting", KFGlobal::Instance()->_app_id->GetZoneId() );
        }

        // 修改名字
        KFMsg::S2SSetPlayerNameToBasicReq req;
        req.set_oldname( name );
        req.set_newname( kfmsg->name() );
        req.set_costdata( _invalid_string );
        req.set_zoneid( setting->_data_id );
        req.set_playerid( entity->GetKeyID() );
        auto ok = _kf_route->SendToRand( entity->GetKeyID(), __ROUTE_NAME__, KFMsg::S2S_SET_PLAYER_NAME_TO_BASIC_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( entity, KFMsg::PublicServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBasicClientModule::HandleSetPlayerNameToGameAck, KFMsg::S2SSetPlayerNameToGameAck )
    {
        _kf_display->SendToClient( entity, kfmsg->result(), kfmsg->name() );
        if ( kfmsg->result() != KFMsg::NameSetOk )
        {
            return;
        }

        entity->UpdateObject( __STRING__( basic ), __STRING__( name ), kfmsg->name() );

        if ( kfmsg->costdata() != _invalid_string )
        {
            KFElements elements;
            auto ok = elements.Parse( kfmsg->costdata(), __FUNC_LINE__ );
            if ( ok )
            {
                entity->RemoveElement( &elements, _default_multiple, __STRING__( name ), 0u, __FUNC_LINE__ );
            }
        }
    }
}
