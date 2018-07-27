#include "KFRobot.h"
#include "KFFSM.h"


namespace KFrame
{
    KFRobot::KFRobot()
    {
        _kf_fsm = new KFFSM( this );
        _kf_fsm->ChangeState( RobotStateEnum::IdleState );
        _state = RobotStateEnum::IdleState;
        _mail_num_arr.clear();
        _matchid = _invalid_int;
        _loop_wait_times = _invalid_int;
        _robot_id = _invalid_int;
        _net_client = nullptr;
        _kf_component = nullptr;
        _playerid = _invalid_int;
        _accountid = _invalid_int;
        _connect_port = 0;
    }

    KFRobot::~KFRobot()
    {
        delete _kf_fsm;
        // 这边不能释放 _net_client指针；

    }

    void KFRobot::ChangeState( uint32 state )
    {
        _kf_fsm->ChangeState( state );
        _state = state;
    }

    void KFRobot::ChangeStateProxy()
    {
        if ( nullptr != _state_list )
        {
            _state_list = _state_list->next;
            ChangeState( _state_list->_state );
        }
    }


    //bool KFRobot::_register_complete = false;

    void KFRobot::Initialize( uint32 robotid, KFNetClientEngine* netclient, KFComponent* kfcomponent )
    {
        _robot_id = robotid;
        _account = __TO_STRING__( robotid );
        _net_client = netclient;
        _kf_component = kfcomponent;

        ChangeState( RobotStateEnum::AuthState );
    }

    void KFRobot::Run()
    {
        _kf_fsm->Run();

    }

    void KFRobot::StartTimer( uint64 spacetime )
    {
        _fsm_timer.StopTimer();
        _fsm_timer.StartTimer( 1, spacetime );
    }

    void KFRobot::StopTimer()
    {
        _fsm_timer.StopTimer();
    }

    bool KFRobot::DoneTimer()
    {
        return _fsm_timer.DoneTimer( KFGlobal::Instance()->_game_time, true );
    }

    void KFRobot::ConnectServer( const std::string& servertype, uint32 serverid, const std::string& name, const std::string& ip, uint32 port )
    {
        _net_client->StartClient( name, servertype, _robot_id, ip, port );
    }

    void KFRobot::DisconnectServer( uint32 serverid )
    {
        _net_client->CloseClient( _robot_id, __FUNCTION_LINE__ );

        _connect_token = "";
        _connect_ip = "";
        _connect_port = 0;
        _gate_server_id = "";
        _gate_server_type = "";
        _gate_ip = "";
        _gate_port = "";
        _gate_name = "";
        _token = "";

        _kf_component->RemoveEntity( _playerid );
    }

    void KFRobot::OnConnectSendMessage()
    {
        if ( RobotStateEnum::LoginVerify == _state )
        {
            KFMsg::MsgLoginVerifyReq req;
            req.set_accountid( _accountid );
            req.set_token( _token );
            SendNetMessage( KFMsg::MSG_LOGIN_VERIFY_REQ, &req );
        }

        else if ( RobotStateEnum::LoginGame == _state )
        {
            KFMsg::MsgLoginGameReq req;
            req.set_playerid( _playerid );
            req.set_token( _connect_token );
            SendNetMessage( KFMsg::MSG_LOGIN_GAME_REQ, &req );
        }
    }

    void KFRobot::SendNetMessage( uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        _net_client->SendNetMessage( _robot_id, msgid, strdata.data(), strdata.size() );
    }

    void KFRobot::CreateRole()
    {
        {
            KFMsg::MsgCreateRoleReq req;
            req.set_name( _account );
            req.set_sex( 1 );
            req.set_model( 1 );
            SendNetMessage( KFMsg::MSG_CREATE_ROLE_REQ, &req );
        }

        {
            KFMsg::MsgChangeSexReq req;
            req.set_sex( 1 );
            SendNetMessage( KFMsg::MSG_CHANGE_SEX_REQ, &req );
        }


    }
    void KFRobot::QueryMailNum()
    {
    }


    void KFRobot::QueryMailInfo()
    {
        KFMsg::MsgQueryMailReq req;
        auto mailtype = KFRand::STRandDistrict( 1, 3, 1 );

        req.set_mailtype( mailtype );
        SendNetMessage( KFMsg::MSG_QUERY_MAIL_REQ, &req );
    }

    void KFRobot::ViewMail()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kfmails = kfobject->FindData( __KF_STRING__( mail ) );

        if ( kfmails == nullptr )
        {
            return;
        }

        auto kfmail = kfmails->FirstData();
        std::vector<uint32> removelist;

        while ( nullptr != kfmail )
        {
            if ( 0 == kfmail->GetValue<uint32>( __KF_STRING__( flag ) ) )
            {
                KFMsg::MsgViewMailReq req;
                req.set_mailid( kfmail->GetValue<uint64>( __KF_STRING__( id ) ) );
                SendNetMessage( KFMsg::MSG_VIEW_MAIL_REQ, &req );
            }

            kfmail = kfmails->NextData();
        }
    }

    void KFRobot::ReceiveMailReward()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kfmails = kfobject->FindData( __KF_STRING__( mail ) );

        if ( kfmails == nullptr )
        {
            return;
        }

        auto kfmail = kfmails->FirstData();

        while ( nullptr != kfmail )
        {
            if ( ( KFMsg::FlagEnum::Received != kfmail->GetValue<uint32>( __KF_STRING__( flag ) )
                    && !kfmail->GetValue<std::string>( __KF_STRING__( reward ) ).empty() ) )
            {
                KFMsg::MsgReceiveMailRewardReq req;
                req.set_mailid( kfmail->GetValue<uint64>( __KF_STRING__( id ) ) );
                SendNetMessage( KFMsg::MSG_RECEIVE_MAIL_REWARD_REQ, &req );
            }

            kfmail = kfmails->NextData();
        }
    }

    void KFRobot::DelMail()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kfmails = kfobject->FindData( __KF_STRING__( mail ) );

        if ( kfmails == nullptr )
        {
            return;
        }

        auto kfmail = kfmails->FirstData();

        while ( nullptr != kfmail )
        {
            if ( kfmail->GetValue<std::string>( __KF_STRING__( reward ) ).empty()
                    || ( KFMsg::FlagEnum::Received == kfmail->GetValue<uint32>( __KF_STRING__( flag ) )
                         && !kfmail->GetValue<std::string>( __KF_STRING__( reward ) ).empty() ) )
            {
                KFMsg::MsgDeleteMailReq req;
                req.set_mailid( kfmail->GetValue<uint64>( __KF_STRING__( id ) ) );
                SendNetMessage( KFMsg::MSG_DELETE_MAIL_REQ, &req );
            }

            kfmail = kfmails->NextData();
        }
    }

    void KFRobot::EnterChat()
    {
        KFMsg::MsgEnterChatReq req;
        SendNetMessage( KFMsg::MSG_ENTER_CHAT_REQ, &req );
    }

    void KFRobot::LeaveChat()
    {
        KFMsg::MsgLeaveChatReq req;
        SendNetMessage( KFMsg::MSG_LEAVE_CHAT_REQ, &req );
    }

    void KFRobot::SendChatMessage()
    {
        KFMsg::MsgSendChatReq req;
        auto messageInfo = _account + ":" + "playerid:" + __TO_STRING__( _playerid );
        req.set_chatinfo( messageInfo );
        req.set_isvoice( 0 );
        SendNetMessage( KFMsg::MSG_SEND_CHAT_REQ, &req );

    }

    void KFRobot::BuyStore()
    {
        for ( uint32 i = 0; i < KFRobotPolicMgr::Instance()->GetStoreSize(); ++i )
        {
            KFMsg::MsgBuyStoreReq req;
            std::string buytype = "";
            uint32 shopid = 0;
            uint32 num = 0;

            if ( KFRobotPolicMgr::Instance()->GetStoreSetting( buytype, shopid, num, i ) )
            {
                req.set_buytype( buytype );
                req.set_shopid( shopid );
                req.set_num( num );
                SendNetMessage( KFMsg::MSG_BUY_STORE_REQ, &req );
            }
        }
    }

    void KFRobot::GiveStore()
    {
        auto fetterplayerid = KFRobotPolicMgr::Instance()->GetFetterRole( _playerid );

        if ( _invalid_int == fetterplayerid )
        {
            //std::cout << "at 【GiveStore】can find fetterplayerid playerid: " << _playerid << std::endl;
            return;
        }

        for ( uint32 i = 0; i < KFRobotPolicMgr::Instance()->GetStoreSize(); ++i )
        {
            KFMsg::MsgGiveStoreReq req;
            std::string buytype = "";
            uint32 shopid = 0;
            uint32 num = 0;

            if ( KFRobotPolicMgr::Instance()->GetStoreSetting( buytype, shopid, num, i ) )
            {
                req.set_shopid( shopid );
                req.set_toplayerid( fetterplayerid );
                SendNetMessage( KFMsg::MSG_GIVE_STORE_REQ, &req );
            }
        }
    }

    void KFRobot::AddFriend()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kffriends = kfobject->FindData( __KF_STRING__( friend ) );

        if ( kffriends == nullptr )
        {
            return;
        }

        auto addfriendcount = KFRand::STRandDistrict( 1, 10, 1 );
        uint32 cursor = 0;
        std::vector<uint32> playerids;

        if ( KFRobotPolicMgr::Instance()->GetAllFetterRole( playerids, _playerid ) )
        {
            for ( auto& iter : playerids )
            {
                if ( cursor >= addfriendcount )
                {
                    break;
                }

                if ( kffriends->FindData( iter ) != nullptr )
                {
                    continue;
                }

                KFMsg::MsgAddFriendInviteReq req;
                req.set_playerid( iter );
                req.set_name( "robot" );
                req.set_message( _account );
                SendNetMessage( KFMsg::MSG_ADD_FRIEND_INVITE_REQ, &req );
                cursor++;
            }
        }

    }

    void KFRobot::AgreeInvite()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kffriendinvites = kfobject->FindData( __KF_STRING__( friendinvite ) );

        if ( kffriendinvites == nullptr )
        {
            return;
        }

        KFMsg::MsgReplyFriendInviteReq req;
        req.set_playerid( 0 );
        req.set_operate( KFMsg::InviteEnum::Consent );
        SendNetMessage( KFMsg::MSG_REPLY_FRIEND_INVITE_REQ, &req );
    }

    void KFRobot::DelFriend()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kffriends = kfobject->FindData( __KF_STRING__( friend ) );

        if ( nullptr == kffriends )
        {
            return;
        }

        auto kffriend = kffriends->FirstData();

        while ( nullptr != kffriend )
        {
            auto playerid = kffriend->GetValue<uint32>( __KF_STRING__( id ) );

            if ( _invalid_int != playerid )
            {
                KFMsg::MsgDelFriendReq req;
                req.set_playerid( playerid );
                SendNetMessage( KFMsg::MSG_DEL_FRIEND_REQ, &req );
            }

            kffriend = kffriends->NextData();
        }
    }

    void KFRobot::SetRefuseFriendInvite()
    {
        KFMsg::MsgSetRefuseFriendInviteReq req;
        req.set_refuse( _kf_robot_config->_friend_switch );
        SendNetMessage( KFMsg::MSG_SET_REFUSE_FRIEND_INVITE_REQ, &req );
    }

    void KFRobot::QueryBasic()
    {
        auto queryplayerid = KFRobotPolicMgr::Instance()->GetFetterRole( _playerid );
        std::string name = "";
        KFRobotPolicMgr::Instance()->GetPlayerName( queryplayerid, name );

        if ( !name.empty() )
        {
            KFMsg::MsgQueryBasicReq req;
            req.set_name( name );
            SendNetMessage( KFMsg::MSG_QUERY_BASIC_REQ, &req );
        }

    }

    void KFRobot::ChangeName()
    {
        KFMsg::MsgChangeNameReq req;
        std::string newname = _account + "robot:";
        req.set_name( newname );
        SendNetMessage( KFMsg::MSG_CHANGE_NAME_REQ, &req );
    }

    void KFRobot::ChangeSex()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kfbasics = kfobject->FindData( __KF_STRING__( basic ) );
        auto sex = kfbasics->GetValue<uint32>( __KF_STRING__( sex ) );
        uint32 newsex = ( sex == _invalid_int ? 1 : _invalid_int );

        KFMsg::MsgChangeSexReq req;
        req.set_sex( newsex );
        SendNetMessage( KFMsg::MSG_CHANGE_SEX_REQ, &req );
    }

    void KFRobot::SendFriendChat()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kffriends = kfobject->FindData( __KF_STRING__( friend ) );

        if ( nullptr == kffriends )
        {
            return;
        }

        auto kffriend = kffriends->FirstData();

        while ( nullptr != kffriend )
        {
            auto playerid = kffriend->GetValue<uint32>( __KF_STRING__( id ) );

            if ( _invalid_int != playerid )
            {   /*
                KFMsg::MsgSendFriendChatInfo req;
                req.set_chatinfo( _account );
                req.set_isvoice( 0 );
                req.set_playerid( playerid );
                SendNetMessage( KFMsg::MSG_SEND_FRIEND_CHAT_REQ, &req );*/
            }

            kffriend = kffriends->NextData();
        }
    }

    void KFRobot::ChangeIcon()
    {
        KFMsg::MsgChangeIconReq req;
        req.set_icon( _account );
        SendNetMessage( KFMsg::MSG_CHANGE_ICON_REQ, &req );
    }

    void KFRobot::ChangeIconBox()
    {
        KFMsg::MsgChangeIconBoxReq req;
        req.set_iconbox( _account );
        SendNetMessage( KFMsg::MSG_CHANGE_ICON_BOX_REQ, &req );
    }

    void KFRobot::ChangeMotto()
    {
        KFMsg::MsgChangeMottoReq req;
        req.set_motto( _account );
        SendNetMessage( KFMsg::MSG_CHANGE_MOTTO_REQ, &req );
    }

    void KFRobot::ChangeModel()
    {
        KFMsg::MsgChangeModelReq req;
        req.set_modelid( 1 );
        SendNetMessage( KFMsg::MSG_CHANGE_MODEL_REQ, &req );

    }

    void KFRobot::ChangeClothes()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kfmodels = kfobject->FindData( __KF_STRING__( model ) );
        auto kfclothes = kfobject->FindData( __KF_STRING__( clothes ) );

        if ( nullptr == kfmodels )
        {
            return;
        }

        auto kfmodel = kfmodels->FirstData();
        auto kfcloth = kfclothes->FirstData();

        while ( nullptr != kfmodel )
        {
            auto modelid = kfmodel->GetValue<uint32>( __KF_STRING__( id ) );

            if ( _invalid_int != modelid )
            {
                while ( nullptr != kfcloth )
                {
                    auto clothid = kfcloth->GetValue<uint32>( __KF_STRING__( id ) );

                    if ( _invalid_int != clothid )
                    {
                        KFMsg::MsgSetModelDefaultClothesReq req;
                        req.set_modelid( modelid );
                        req.set_clothesid( clothid );
                        SendNetMessage( KFMsg::MSG_SET_MODEL_DEFAULT_CLOTHES_REQ, &req );
                    }

                    kfcloth = kfclothes->NextData();
                }
            }

            kfmodel = kfmodels->NextData();
        }
    }

    void KFRobot::DressClothes()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kfclothes = kfobject->FindData( __KF_STRING__( clothes ) );
        auto kfcloth = kfclothes->FirstData();

        while ( nullptr != kfcloth )
        {
            auto clothid = kfcloth->GetValue<uint32>( __KF_STRING__( id ) );

            if ( _invalid_int != clothid )
            {
                KFMsg::MsgDressClothesReq req;
                req.set_clothesid( clothid );
                SendNetMessage( KFMsg::MSG_DRESS_CLOTHES_REQ, &req );
            }

            kfcloth = kfclothes->NextData();
        }
    }

    void KFRobot::QueryGuest()
    {
        std::vector<uint32> playerids;

        if ( KFRobotPolicMgr::Instance()->GetAllFetterRole( playerids, _playerid ) )
        {
            for ( auto& iter : playerids )
            {
                KFMsg::MsgQueryGuestReq req;
                req.set_playerid( iter );
                SendNetMessage( KFMsg::MSG_QUERY_GUEST_REQ, &req );
            }
        }
    }

    void KFRobot::RecvActivityReward()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kfactivitys = kfobject->FindData( __KF_STRING__( activity ) );
        auto kfactivity = kfactivitys->FirstData();

        while ( nullptr != kfactivity )
        {
            auto id = kfactivity->GetValue<uint32>( __KF_STRING__( id ) );
            auto type = kfactivity->GetValue<uint32>( __KF_STRING__( type ) );
            KFMsg::MsgReceiveActivityRewardReq req;
            req.set_id( id );
            req.set_type( type );
            SendNetMessage( KFMsg::MSG_RECEIVE_ACTIVITY_REWARD_REQ, &req );
            kfactivity = kfactivitys->NextData();
        }
    }

    void KFRobot::RecvTaskReward()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kftasks = kfobject->FindData( __KF_STRING__( task ) );
        auto kftask = kftasks->FirstData();

        while ( nullptr != kftask )
        {
            auto taskflag = kftask->GetValue<uint32>( __KF_STRING__( flag ) );
            auto taskid = kftask->GetKeyID();

            if ( KFMsg::FlagEnum::Done == taskflag )
            {
                KFMsg::MsgReceiveTaskRewardReq req;
                req.set_taskid( taskid );
                SendNetMessage( KFMsg::MSG_RECEIVE_TASK_REWARD_REQ, &req );
            }

            kftask = kftasks->NextData();
        }
    }

    void KFRobot::RecvAchieveReward()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kftasks = kfobject->FindData( __KF_STRING__( achieve ) );
        auto kftask = kftasks->FirstData();

        while ( nullptr != kftask )
        {
            auto taskflag = kftask->GetValue<uint32>( __KF_STRING__( flag ) );
            auto taskid = kftask->GetKeyID();

            if ( KFMsg::FlagEnum::Done == taskflag )
            {
                KFMsg::MsgReceiveAchieveRewardReq req;
                req.set_achieveid( taskid );
                SendNetMessage( KFMsg::MSG_RECEIVE_ACHIEVE_REWARD_REQ, &req );
            }

            kftask = kftasks->NextData();
        }
    }

    void KFRobot::RecvGiftReward()
    {
        KFMsg::MsgReceiveGiftRewardReq req;

        if ( !_kf_robot_config->_cd_key.empty() )
        {
            req.set_giftkey( _kf_robot_config->_cd_key );
            SendNetMessage( KFMsg::MSG_RECEIVE_GIFT_REWARD_REQ, &req );
        }
    }

    void KFRobot::Toast()
    {
        std::vector<uint32> playerids;
        auto addToast = KFRand::STRandDistrict( 1, 10, 1 );
        uint32 cursor = 0;

        if ( KFRobotPolicMgr::Instance()->GetAllFetterRole( playerids, _playerid ) )
        {
            for ( auto& iter : playerids )
            {
                if ( cursor >= addToast )
                {
                    break;
                }

                KFMsg::MsgPlayerToastReq req;
                req.set_playerid( iter );
                SendNetMessage( KFMsg::MSG_PLAYER_TOAST_REQ, &req );
                ++addToast;

            }
        }
    }

    void KFRobot::InviteTeam()
    {
        std::vector<uint32> playerids;
        auto addToast = KFRand::STRandDistrict( 1, 10, 1 );
        uint32 cursor = 0;

        if ( KFRobotPolicMgr::Instance()->GetAllFetterRole( playerids, _playerid ) )
        {
            for ( auto& iter : playerids )
            {
                if ( cursor >= addToast )
                {
                    break;
                }

                KFMsg::MsgInviteMatchGroupReq req;
                req.set_playerid( iter );
                auto inviteplayer = KFRobotPolicMgr::Instance()->FindRoleById( iter );

                if ( nullptr != inviteplayer )
                {
                    auto kfobject = inviteplayer->GetData();
                    auto kfbasics = kfobject->FindData( __KF_STRING__( basic ) );
                    auto serverid = kfbasics->GetValue<uint32>( __KF_STRING__( serverid ) );
                    req.set_serverid( serverid );
                    SendNetMessage( KFMsg::MSG_INVITE_MATCH_GROUP_REQ, &req );
                    cursor++;
                }
            }
        }
    }

    void KFRobot::AgreeInviteTeam()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kfinviterecord = kfobject->FindData( __KF_STRING__( groupinvite ) );
        auto kfinvite = kfinviterecord->FirstData();

        while ( nullptr != kfinvite )
        {
            auto groupid = kfinvite->GetValue<uint64>( __KF_STRING__( groupid ) );
            auto inviterid = kfinvite->GetValue<uint32>( __KF_STRING__( id ) );
            KFMsg::MsgReplyInviteMatchGroupReq req;
            req.set_groupid( groupid );
            req.set_inviterid( inviterid );
            req.set_operate( KFMsg::InviteEnum::Consent );
            SendNetMessage( KFMsg::MSG_REPLY_INVITE_MATCH_GROUP_REQ, &req );
            kfinvite = kfinviterecord->NextData();
        }
    }

    void KFRobot::RefuseInviteTeam()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kfinviterecord = kfobject->FindData( __KF_STRING__( groupinvite ) );
        auto kfinvite = kfinviterecord->FirstData();

        while ( nullptr != kfinvite )
        {
            auto groupid = kfinvite->GetValue<uint64>( __KF_STRING__( groupid ) );
            auto inviterid = kfinvite->GetValue<uint32>( __KF_STRING__( id ) );
            KFMsg::MsgReplyInviteMatchGroupReq req;
            req.set_groupid( groupid );
            req.set_inviterid( inviterid );
            req.set_operate( KFMsg::InviteEnum::Refuse );
            SendNetMessage( KFMsg::MSG_REPLY_INVITE_MATCH_GROUP_REQ, &req );
            kfinvite = kfinviterecord->NextData();
        }
    }

    void KFRobot::RefuseMinuInviteTeam()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kfinviterecord = kfobject->FindData( __KF_STRING__( groupinvite ) );
        auto kfinvite = kfinviterecord->FirstData();

        while ( nullptr != kfinvite )
        {
            auto groupid = kfinvite->GetValue<uint64>( __KF_STRING__( groupid ) );
            auto inviterid = kfinvite->GetValue<uint32>( __KF_STRING__( id ) );
            KFMsg::MsgReplyInviteMatchGroupReq req;
            req.set_groupid( groupid );
            req.set_inviterid( inviterid );
            req.set_operate( KFMsg::InviteEnum::RefuseMinute );
            SendNetMessage( KFMsg::MSG_REPLY_INVITE_MATCH_GROUP_REQ, &req );
            kfinvite = kfinviterecord->NextData();
        }
    }

    void KFRobot::LeaveTeam()
    {
        KFMsg::MsgLeaveMatchGroupReq req;
        SendNetMessage( KFMsg::MSG_LEAVE_MATCH_GROUP_REQ, &req );
    }

    void KFRobot::ApplyGroup()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kffriends = kfobject->FindData( __KF_STRING__( friend ) );
        auto kffriend = kffriends->FirstData();

        while ( nullptr != kffriend )
        {
            auto kfbasic = kffriend->FindData( __KF_STRING__( basic ) );
            auto groupid = kfbasic->GetValue<uint64>( __KF_STRING__( groupid ) );

            if ( _invalid_int != groupid )
            {
                KFMsg::MsgApplyMatchGroupReq req;
                req.set_groupid( groupid );
                SendNetMessage( KFMsg::MSG_APPLY_MATCH_GROUP_REQ, &req );
            }

            kffriend = kffriends->NextData();
        }
    }

    void KFRobot::AgreeApplyTeam()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kfapplyrecord = kfobject->FindData( __KF_STRING__( groupapply ) );
        auto kfapply = kfapplyrecord->FirstData();

        while ( nullptr != kfapply )
        {
            auto applyid = kfapply->GetValue<uint32>( __KF_STRING__( id ) );

            if ( _invalid_int != applyid )
            {
                KFMsg::MsgReplyApplyMatchGroupReq req;
                req.set_applyid( applyid );
                req.set_operate( KFMsg::InviteEnum::Consent );
                SendNetMessage( KFMsg::MSG_REPLY_APPLY_MATCH_GROUP_REQ, &req );
            }

        }
    }

    void KFRobot::RefuseApplyTeam()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kfapplyrecord = kfobject->FindData( __KF_STRING__( groupapply ) );
        auto kfapply = kfapplyrecord->FirstData();

        while ( nullptr != kfapply )
        {
            auto applyid = kfapply->GetValue<uint32>( __KF_STRING__( id ) );

            if ( _invalid_int != applyid )
            {
                KFMsg::MsgReplyApplyMatchGroupReq req;
                req.set_applyid( applyid );
                req.set_operate( KFMsg::InviteEnum::Refuse );
                SendNetMessage( KFMsg::MSG_REPLY_APPLY_MATCH_GROUP_REQ, &req );
            }

        }
    }

    void KFRobot::KickGroup()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();

        auto kfgroupmembers = kfobject->FindData( __KF_STRING__( group ), __KF_STRING__( groupmember ) );
        auto kfgroupmember = kfgroupmembers->FirstData();

        while ( nullptr != kfgroupmember )
        {
            auto memberid = kfgroupmember->GetValue<uint32>( __KF_STRING__( id ) );

            if ( _invalid_int != memberid )
            {
                KFMsg::MsgKickMatchGroupReq req;
                req.set_memberid( memberid );
                SendNetMessage( KFMsg::MSG_KICK_MATCH_GROUP_REQ, &req );
            }
        }
    }

    void KFRobot::SingleMatch()
    {
        StartMatch( KFRObotEnum::Single_Match, false );
    }


    void KFRobot::CancleMatch()
    {
        KFMsg::MsgCancelMatchReq req;
        SendNetMessage( KFMsg::MSG_CANCEL_MATCH_REQ, &req );
    }

    void KFRobot::ParsePB2KFData( KFEntity* player, const KFMsg::PBObject* pbobject, KFData*& kfparent, uint64& key )
    {
        if ( !pbobject->name().empty() )
        {
            if ( pbobject->name() != "player" )
            {
                if ( kfparent->FindData( pbobject->name() ) )
                {
                    kfparent = kfparent->FindData( pbobject->name() );
                }

            }
        }

        if ( pbobject->key() != 0 )
        {
            key = pbobject->key();
        }

        for ( auto i = 0; i < pbobject->pbobject_size(); ++i )
        {
            ParsePB2KFData( player, &pbobject->pbobject( i ), kfparent, key );
        }

        for ( auto i = 0; i < pbobject->pbrecord_size(); ++i )
        {
            auto record = &pbobject->pbrecord( i );

            if ( !record->name().empty() )
            {
                if ( nullptr != kfparent )
                {
                    kfparent = kfparent->FindData( record->name() );
                }

                if ( nullptr == kfparent )
                {
                    // log
                    KFLogger::LogLogic( KFLogger::Error, "[%s] robot[%u] parse pb to kfdata Error[%s]!",
                                        __FUNCTION__, player->GetKeyID(), record->name().c_str() );
                }

            }

            for ( auto j = 0; j < record->pbobject_size(); ++j )
            {
                ParsePB2KFData( player, &record->pbobject( j ), kfparent, key );
                player->RemoveData( kfparent, key );
            }
        }
    }

    void KFRobot::QueryPlayer()
    {
        auto queryplayerid = KFRobotPolicMgr::Instance()->GetFetterRole( _playerid );

        if ( _invalid_int != queryplayerid )
        {
            KFMsg::MsgQueryPlayerReq req;
            req.set_playerid( queryplayerid );
            SendNetMessage( KFMsg::MSG_QUERY_PLAYER_REQ, &req );
        }
    }

    void KFRobot::QueryStoreVersion()
    {
        //KFMsg::MsgQueryVersionReq req;
        //SendNetMessage( KFMsg::MSG_QUERY_VERSION_REQ, &req );
    }


    void KFRobot::QueryStoreInfo()
    {
        KFMsg::MsgQueryStoreInfoReq req;
        req.set_version( 2 );
        SendNetMessage( KFMsg::MSG_QUERY_STORE_INFO_REQ, &req );
    }

    void KFRobot::QueryWholeRankList()
    {   /*
           KFMsg::MsgQueryWholeRankListReq req;
           req.set_matchid( 4 );
           req.set_ranktype( KFField::_eval_rank );
           SendNetMessage( KFMsg::MSG_QUERY_WHOLE_RANK_LIST_REQ, &req );*/
    }

    void KFRobot::QueryFriendRankList()
    {

        KFMsg::MsgQueryFriendRankListReq req;

        req.set_rankid( 1 );
        SendNetMessage( KFMsg::MSG_QUERY_FRIEND_RANK_LIST_REQ, &req );
    }

    void KFRobot::ParseUpdatePB2KFData( KFEntity* player, const KFMsg::PBObject* pbobject, KFData*& kfparent, uint64& key )
    {
        if ( nullptr == kfparent )
        {
            return;
        }

        if ( !pbobject->name().empty() )
        {
            if ( pbobject->name() != "player" )
            {
                if ( kfparent->FindData( pbobject->name() ) )
                {
                    kfparent = kfparent->FindData( pbobject->name() );
                }

            }
        }

        if ( pbobject->key() != 0 )
        {
            key = pbobject->key();
        }

        for ( auto i = 0; i < pbobject->pbstring_size(); ++i )
        {
            auto pbstring = &pbobject->pbstring( i );
            auto name = pbstring->name();
            auto value = pbstring->value();

            if ( kfparent->FindData( key ) )
            {
                player->UpdateData( kfparent->FindData( key ), name, value );
            }

            else
            {
                player->UpdateData( name, value );
            }

        }

        for ( auto i = 0; i < pbobject->pbuint64_size(); ++i )
        {
            auto pbuint64 = &pbobject->pbuint64( i );
            auto name = pbuint64->name();
            auto value = pbuint64->value();

            if ( kfparent->FindData( key ) )
            {
                player->UpdateData( kfparent->FindData( key ), name, __TO_STRING__( value ) );
            }

            else
            {
                player->UpdateData( name, __TO_STRING__( value ) );
            }

        }

        for ( auto i = 0; i < pbobject->pbuint32_size(); ++i )
        {
            auto pbuint64 = &pbobject->pbuint32( i );
            auto name = pbuint64->name();
            auto value = pbuint64->value();

            if ( kfparent->FindData( key ) )
            {
                player->UpdateData( kfparent->FindData( key ), name, __TO_STRING__( value ) );
            }

            else
            {
                player->UpdateData( name, __TO_STRING__( value ) );
            }
        }

        for ( auto i = 0; i < pbobject->pbint32_size(); ++i )
        {
            auto pbuint64 = &pbobject->pbint32( i );
            auto name = pbuint64->name();
            auto value = pbuint64->value();

            if ( kfparent->FindData( key ) )
            {
                player->UpdateData( kfparent->FindData( key ), name, __TO_STRING__( value ) );
            }

            else
            {
                player->UpdateData( name, __TO_STRING__( value ) );
            }

        }

        for ( auto i = 0; i < pbobject->pbint64_size(); ++i )
        {
            auto pbuint64 = &pbobject->pbint64( i );
            auto name = pbuint64->name();
            auto value = pbuint64->value();

            if ( kfparent->FindData( key ) )
            {
                player->UpdateData( kfparent->FindData( key ), name, __TO_STRING__( value ) );
            }

            else
            {
                player->UpdateData( name, __TO_STRING__( value ) );
            }
        }

        for ( auto i = 0; i < pbobject->pbdouble_size(); ++i )
        {
            auto pbuint64 = &pbobject->pbdouble( i );
            auto name = pbuint64->name();
            auto value = pbuint64->value();

            if ( kfparent->FindData( key ) )
            {
                player->UpdateData( kfparent->FindData( key ), name, __TO_STRING__( value ) );
            }

            else
            {
                player->UpdateData( name, __TO_STRING__( value ) );
            }
        }

        for ( auto i = 0; i < pbobject->pbobject_size(); ++i )
        {
            ParseUpdatePB2KFData( player, &pbobject->pbobject( i ), kfparent, key );
        }

        for ( auto i = 0; i < pbobject->pbrecord_size(); ++i )
        {
            auto record = &pbobject->pbrecord( i );

            if ( !record->name().empty() )
            {
                if ( kfparent->FindData( record->name() ) )
                {
                    kfparent = kfparent->FindData( record->name() );
                }


            }

            for ( auto j = 0; j < record->pbobject_size(); ++j )
            {
                ParseUpdatePB2KFData( player, &record->pbobject( j ), kfparent, key );
            }
        }

    }


    void KFRobot::StartMatch( uint32 mode, bool group )
    {
        KFMsg::MsgStartMatchReq req;
        req.set_matchid( mode );
        req.set_allowgroup( group );
        SendNetMessage( KFMsg::MSG_START_MATCH_REQ, &req );
        _matchid = mode;
    }

    void KFRobot::AddMoney()
    {
        KFMsg::MsgDebugCommandReq req;
        req.set_command( "AddData" );
        req.add_params( "[{\"money\":{\"money\":3000}}]" );
        SendNetMessage( KFMsg::MSG_DEBUG_COMMAND_REQ, &req );
    }

    void KFRobot::AddDiamond()
    {
        KFMsg::MsgDebugCommandReq req;
        req.set_command( "AddData" );
        req.add_params( "[{\"diamond\":{\"diamond\":3000}}]" );
        SendNetMessage( KFMsg::MSG_DEBUG_COMMAND_REQ, &req );
    }

    void KFRobot::AddRecent()
    {
        KFMsg::MsgDebugCommandReq req;
        req.set_command( "AddRecent" );

        auto randuid = KFRand::STRandDistrict( 1010001, 1014352, 1 );
        req.add_params( __TO_STRING__( randuid ) );
        SendNetMessage( KFMsg::MSG_DEBUG_COMMAND_REQ, &req );
    }

    void KFRobot::AddWishOrder()
    {
        for ( uint32 i = 0; i < KFRobotPolicMgr::Instance()->GetStoreSize(); ++i )
        {
            KFMsg::MsgSetWishOrderReq req;
            std::string buytype = "";
            uint32 shopid = 0;
            uint32 num = 0;

            if ( KFRobotPolicMgr::Instance()->GetStoreSetting( buytype, shopid, num, i ) )
            {
                req.set_type( KFMsg::StoreWishEnum::Add );
                req.set_storeid( shopid );
                req.set_status( KFMsg::WishStateEnum::AllWatch );
                SendNetMessage( KFMsg::MSG_SET_WISH_ORDER_REQ, &req );
            }
        }
    }

    void KFRobot::DelWishOrder()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kfwishorders = kfobject->FindData( __KF_STRING__( wishorder ) );
        auto kfwishorder = kfwishorders->FirstData();

        while ( nullptr != kfwishorder )
        {
            auto wishorderid = kfwishorder->GetValue<uint32>( __KF_STRING__( id ) );

            if ( _invalid_int != wishorderid )
            {
                KFMsg::MsgSetWishOrderReq req;
                req.set_storeid( wishorderid );
                req.set_type( KFMsg::StoreWishEnum::Del );
                SendNetMessage( KFMsg::MSG_SET_WISH_ORDER_REQ, &req );

            }
        }
    }

    void KFRobot::ModifyWishOrder()
    {
        auto player = _kf_component->FindEntity( _playerid, __FUNCTION_LINE__ );

        if ( nullptr == player )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kfwishorders = kfobject->FindData( __KF_STRING__( wishorder ) );
        auto kfwishorder = kfwishorders->FirstData();

        while ( nullptr != kfwishorder )
        {
            auto wishorderid = kfwishorder->GetValue<uint32>( __KF_STRING__( id ) );
            auto status = kfwishorder->GetValue<uint32>( __KF_STRING__( status ) );

            if ( _invalid_int != wishorderid )
            {
                KFMsg::MsgSetWishOrderReq req;
                req.set_storeid( wishorderid );
                req.set_type( KFMsg::StoreWishEnum::Modify );
                uint32 newstatus = ( status == KFMsg::WishStateEnum::AllWatch ? KFMsg::WishStateEnum::FriendWatch : KFMsg::WishStateEnum::AllWatch );
                req.set_status( newstatus );
                SendNetMessage( KFMsg::MSG_SET_WISH_ORDER_REQ, &req );
            }
        }
    }
}
