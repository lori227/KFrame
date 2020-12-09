#include "KFBusModule.hpp"

namespace KFrame
{
    void KFBusModule::BeforeRun()
    {
        __REGISTER_TCP_CLIENT_FAILED__( &KFBusModule::OnClientConnectMasterFailed );
        __REGISTER_TCP_CLIENT_SHUTDOWN__( &KFBusModule::OnClientDisconnectMaster );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFBusModule, KFMessageEnum::Normal, KFMsg::S2S_TELL_REGISTER_TO_SERVER, KFMsg::TellRegisterToServer, HanldeTellRegisterToServer );
        __REGISTER_MESSAGE__( KFBusModule, KFMessageEnum::Normal, KFMsg::S2S_TELL_UNREGISTER_FROM_SERVER, KFMsg::TellUnRegisterFromServer, HanldeTellUnRegisterFromServer );
    }

    void KFBusModule::ShutDown()
    {
        __UN_TCP_CLIENT_FAILED__();
        __UN_TCP_CLIENT_SHUTDOWN__();
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_TELL_REGISTER_TO_SERVER );
        __UN_MESSAGE__( KFMsg::S2S_TELL_UNREGISTER_FROM_SERVER );
    }

    void KFBusModule::PrepareRun()
    {
        // 如果是master, 不执行
        auto kfglobal = KFGlobal::Instance();
        if ( kfglobal->_app_type == __STRING__( master ) )
        {
            return;
        }

        // 查找需要连接的master服务器, 自动连接
        auto kfsetting = FindMasterConnection( kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id->ToString() );
        if ( kfsetting != nullptr )
        {
            // 启动定时器
            __LOOP_TIMER_0__( 6000u, 0u, &KFBusModule::OnTimerConnectionMaster );
        }
    }

    __KF_TIMER_FUNCTION__( KFBusModule::OnTimerConnectionMaster )
    {
        auto kfglobal = KFGlobal::Instance();
        auto kfaddress = _kf_ip_address->GetMasterIp( kfglobal->_app_name, kfglobal->_app_id->GetZoneId() );
        if ( kfaddress == nullptr )
        {
            return;
        }

        __UN_TIMER_0__();
        _connect_master_failed_count = 0u;
        _kf_tcp_client->StartClient( kfaddress->_name, kfaddress->_type, kfaddress->_id, kfaddress->_ip, kfaddress->_port );
    }

    __KF_NET_EVENT_FUNCTION__( KFBusModule::OnClientConnectMasterFailed )
    {
        if ( netdata->_name == KFGlobal::Instance()->_app_name &&
                netdata->_type == __STRING__( master ) )
        {
            ++_connect_master_failed_count;
            if ( _connect_master_failed_count > 5u )
            {
                // 超过设定次数, 重新连接
                _kf_tcp_client->CloseClient( netdata->_id, __FUNC_LINE__ );
            }
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFBusModule::OnClientDisconnectMaster )
    {
        if ( netdata->_name == KFGlobal::Instance()->_app_name &&
                netdata->_type == __STRING__( master ) )
        {
            // 启动定时器
            __LOOP_TIMER_0__( 6000u, 1000u, &KFBusModule::OnTimerConnectionMaster );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 通知有客户端注册
    __KF_MESSAGE_FUNCTION__( KFBusModule::HanldeTellRegisterToServer, KFMsg::TellRegisterToServer )
    {
        auto listendata = &kfmsg->listen();
        if ( !CheckNeedConnection( listendata->appname(), listendata->apptype(), listendata->appid() ) )
        {
            return;
        }

        _kf_tcp_client->StartClient( listendata->appname(), listendata->apptype(), listendata->appid(), listendata->ip(), listendata->port() );
    }

    __KF_MESSAGE_FUNCTION__( KFBusModule::HanldeTellUnRegisterFromServer, KFMsg::TellUnRegisterFromServer )
    {
        if ( !CheckNeedConnection( kfmsg->appname(), kfmsg->apptype(), kfmsg->appid() ) )
        {
            return;
        }

        _kf_tcp_client->CloseClient( kfmsg->appid(), __FUNC_LINE__ );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFBusModule::CheckNeedConnection( const std::string& connectname, const std::string& connecttype, uint64 connectid )
    {
        auto kfglobal = KFGlobal::Instance();

        // 不是同一类服务器
        // 如果是master, 返回false, 因为master会主动连, 不需要再这里再次连接
        // 自己不能连接自己
        if ( connectname != kfglobal->_app_name ||
                connecttype == __STRING__( master ) ||
                connectid == kfglobal->_app_id->GetId() )
        {
            return false;
        }

        auto strconnectid = KFAppId::ToString( connectid );
        for ( auto& iter : KFBusConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_app_name != _globbing_string && kfsetting->_app_name != kfglobal->_app_name )
            {
                continue;
            }

            if ( kfsetting->_app_type != _globbing_string && kfsetting->_app_type != kfglobal->_app_type )
            {
                continue;
            }

            if ( kfsetting->_app_id != _globbing_string && kfsetting->_app_id != kfglobal->_app_id->ToString() )
            {
                continue;
            }

            // 判断连接目标信息
            if ( kfsetting->_connect_name != _globbing_string && kfsetting->_connect_name != connectname )
            {
                continue;
            }

            if ( kfsetting->_connect_type != _globbing_string && kfsetting->_connect_type != connecttype )
            {
                continue;
            }

            if ( kfsetting->_connect_id != _globbing_string && kfsetting->_connect_id != strconnectid )
            {
                continue;
            }

            // 连接间隔
            if ( kfsetting->_interval != _invalid_int && kfsetting->_multi != _invalid_int )
            {
                KFAppId kfappid( connectid );
                auto connectworkid = kfappid.GetWorkId();
                auto workid = ( kfglobal->_app_id->GetWorkId() + kfsetting->_multi - 1 ) / kfsetting->_multi;
                if ( ( __MAX__( workid, connectworkid ) ) - ( __MIN__( workid, connectworkid ) ) > kfsetting->_interval )
                {
                    return false;
                }
            }

            return true;
        }

        return false;
    }

    const KFBusSetting* KFBusModule::FindMasterConnection( const std::string& appname, const std::string& apptype, const std::string& appid )
    {
        for ( auto& iter : KFBusConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_app_name != _globbing_string && kfsetting->_app_name != appname )
            {
                continue;
            }

            if ( kfsetting->_app_type != _globbing_string && kfsetting->_app_type != apptype )
            {
                continue;
            }

            if ( kfsetting->_app_id != _globbing_string && kfsetting->_app_id != appid )
            {
                continue;
            }

            if ( kfsetting->_connect_name != _globbing_string && kfsetting->_app_name != appname )
            {
                continue;
            }

            if ( kfsetting->_connect_type != _globbing_string && kfsetting->_connect_type != __STRING__( master ) )
            {
                continue;
            }

            return kfsetting;
        }

        return nullptr;
    }
}
