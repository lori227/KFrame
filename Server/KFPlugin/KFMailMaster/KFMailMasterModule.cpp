#include "KFMailMasterModule.h"
#include "KFJson.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFMailMasterModule::InitModule()
    {
    }

    void KFMailMasterModule::BeforeRun()
    {

        __REGISTER_SERVER_LOST_FUNCTION__( &KFMailMasterModule::OnClientLostServer );
        __REGISTER_SERVER_DISCOVER_FUNCTION__( &KFMailMasterModule::OnServerDiscoverClient );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( addmail ), true, &KFMailMasterModule::HandleAddMail );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFMailMasterModule::BeforeShut()
    {
        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( addmail ) );
    }

    void KFMailMasterModule::OnceRun()
    {


    }

    __KF_HTTP_FUNCTION__( KFMailMasterModule::HandleAddMail )
    {
        try
        {
            KFJson response;
            KFJson request( data );
            KFMsg::S2SGMAddMailReq req;
            if ( data.empty() )
            {
                return _invalid_str;
            }

            __LOG_INFO__( KFLogEnum::GM, "get gm addmail msg:{}", request.Serialize() );

            std::vector<uint32> playerids;
            auto strplayerids = request.GetString( __KF_STRING__( playerids ) );
            if ( request.isMember( __KF_STRING__( playerids ) ) && !strplayerids.empty() )
            {
                auto temp = KFUtility::SplitString( strplayerids, "," );
                while ( !temp.empty() )
                {
                    playerids.push_back( KFUtility::ToValue<uint32>( temp ) );
                    temp = KFUtility::SplitString( strplayerids, "," );
                }

                request.removeMember( __KF_STRING__( playerids ) );
            }


            // 设置接收玩家id
            if ( playerids.size() > _invalid_int )
            {
                auto pbplayerids = req.mutable_playerids();
                for ( auto iter : playerids )
                {
                    pbplayerids->add_playerid( iter );
                }
            }

            auto mailtype = request.GetUInt32( __KF_STRING__( type ) );
            if ( KFMsg::MailEnum_MIN > mailtype ||
                    KFMsg::MailEnum_MAX < mailtype )
            {
                return _invalid_str;
            }

            req.set_mailtype( mailtype );

            auto pbmail = req.mutable_pbmail();
            for ( auto iter = request.begin(); iter != request.end(); ++iter )
            {
                auto pbdata = pbmail->add_data();
                pbdata->set_name( iter.name() );
                pbdata->set_value( iter->asString() );
            }
            auto serverid = _kf_hash.FindHashNode( data );
            if ( pbmail->data_size() > 0 )
            {
                _kf_tcp_server->SendNetMessage( serverid, KFMsg::S2S_GM_ADD_MAIL_REQ, &req );
            }

            response.SetValue( __KF_STRING__( retcode ), KFMsg::Success );
            return _kf_http_server->SendResponse( response );
        }
        catch ( ... )
        {
            return _invalid_str;
        }

    }

    __KF_SERVER_DISCOVER_FUNCTION__( KFMailMasterModule::OnServerDiscoverClient )
    {
        auto kfglobal = KFGlobal::Instance();
        if ( kfglobal->_app_name != handlename )
        {
            return;
        }
        if ( handletype != __KF_STRING__( shard ) )
        {
            return;
        }

        _kf_hash.AddHashNode( handlename, handleid, 100 );
    }

    __KF_SERVER_LOST_FUNCTION__( KFMailMasterModule::OnClientLostServer )
    {
        auto kfglobal = KFGlobal::Instance();
        if ( kfglobal->_app_name != handlename )
        {
            return;
        }
        if ( handletype != __KF_STRING__( shard ) )
        {
            return;
        }
        _kf_hash.RemoveHashNode( handleid );
    }
}