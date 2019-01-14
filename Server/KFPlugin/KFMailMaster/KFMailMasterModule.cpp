#include "KFMailMasterModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFMailMasterModule::InitModule()
    {
    }

    void KFMailMasterModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST_FUNCTION__( &KFMailMasterModule::OnServerLostClient );
        __REGISTER_SERVER_DISCOVER_FUNCTION__( &KFMailMasterModule::OnServerDiscoverClient );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //__REGISTER_HTTP_FUNCTION__( __KF_STRING__( addmail ), true, &KFMailMasterModule::HandleAddMail );
    }

    void KFMailMasterModule::BeforeShut()
    {
        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //__UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( addmail ) );
    }

    __KF_SERVER_DISCOVER_FUNCTION__( KFMailMasterModule::OnServerDiscoverClient )
    {
        auto kfglobal = KFGlobal::Instance();
        if ( kfglobal->_app_name != handlename || handletype != __KF_STRING__( shard ) )
        {
            return;
        }

        _kf_hash.AddHashNode( handlename, handleid, 100 );
    }

    __KF_SERVER_LOST_FUNCTION__( KFMailMasterModule::OnServerLostClient )
    {
        auto kfglobal = KFGlobal::Instance();
        if ( kfglobal->_app_name != handlename || handletype != __KF_STRING__( shard ) )
        {
            return;
        }

        _kf_hash.RemoveHashNode( handleid );
    }

    __KF_COMMAND_FUNCTION__( KFMailMasterModule::OnCommandAddMail )
    {
        __JSON_PARSE_STRING__( request, param );

        auto mailtype = __JSON_GET_UINT32__( request, __KF_STRING__( type ) );
        if ( mailtype < KFMsg::MailEnum_MIN || mailtype > KFMsg::MailEnum_MAX )
        {
            __LOG_INFO__( "add gm mail type[{}] error!", mailtype );
            return ;
        }

        KFMsg::S2SGMAddMailReq req;
        req.set_mailtype( mailtype );

        // 邮件玩家列表
        auto strplayerids = __JSON_GET_STRING__( request, __KF_STRING__( playerids ) );
        while ( !strplayerids.empty() )
        {
            auto playerid = KFUtility::SplitValue< uint32 >( strplayerids, "," );
            if ( playerid != _invalid_int )
            {
                req.add_playerids( playerid );
            }
        }

        __JOSN_REMOVE__( request, __KF_STRING__( playerids ) );

        // 邮件内容
        auto pbmail = req.mutable_pbmail();
        for ( auto iter = request.MemberBegin(); iter != request.MemberEnd(); ++iter )
        {
            auto pbdata = pbmail->add_data();
            pbdata->set_name( iter->name.GetString() );

            // 删除空格和%
            std::string strdata = iter->value.GetString();
            KFUtility::ReplaceString( strdata, " ", "" );
            KFUtility::ReplaceString( strdata, "%", "" );
            pbdata->set_value( strdata );
        }

        auto serverid = _kf_hash.FindHashNode( param );
        auto ok = _kf_tcp_server->SendNetMessage( serverid, KFMsg::S2S_GM_ADD_MAIL_REQ, &req );
        if ( ok )
        {
            __LOG_INFO__( "add gm mail server[{}] ok!", serverid );
        }
        else
        {
            __LOG_ERROR__( "add gm mail server[{}] failed!", serverid );
        }
    }

}