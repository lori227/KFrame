#include "KFBusModule.hpp"

namespace KFrame
{
    void KFBusModule::BeforeRun()
    {
        __REGISTER_TCP_CLIENT_FAILED__( &KFBusModule::OnClientConnectMasterFailed );
        __REGISTER_TCP_CLIENT_SHUTDOWN__( &KFBusModule::OnClientDisconnectMaster );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_TELL_REGISTER_TO_SERVER, &KFBusModule::HanldeTellRegisterToServer );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_TELL_UNREGISTER_FROM_SERVER, &KFBusModule::HanldeTellUnRegisterFromServer );
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
        auto global = KFGlobal::Instance();
        if ( global->_app_type == __STRING__( master ) )
        {
            return;
        }

        // 查找需要连接的master服务器, 自动连接
        auto setting = FindMasterConnection( global->_app_name, global->_app_type, global->_app_id->ToString() );
        if ( setting != nullptr )
        {
            // 启动定时器
            __LOOP_TIMER_0__( 6000u, 0u, &KFBusModule::OnTimerConnectionMaster );
        }
    }

    __KF_TIMER_FUNCTION__( KFBusModule::OnTimerConnectionMaster )
    {
        auto global = KFGlobal::Instance();
        auto master_address = _kf_ip_address->GetMasterIp( global->_app_name, global->_app_id->GetZoneId() );
        if ( master_address == nullptr )
        {
            return;
        }

        __UN_TIMER_0__();
        _connect_master_failed_count = 0u;
        _kf_tcp_client->StartClient( master_address->_name, master_address->_type, master_address->_id, master_address->_ip, master_address->_port );
    }

    __KF_NET_EVENT_FUNCTION__( KFBusModule::OnClientConnectMasterFailed )
    {
        if ( net_data->_name == KFGlobal::Instance()->_app_name &&
                net_data->_type == __STRING__( master ) )
        {
            ++_connect_master_failed_count;
            if ( _connect_master_failed_count > 5u )
            {
                // 超过设定次数, 重新连接
                _kf_tcp_client->CloseClient( net_data->_id, __FUNC_LINE__ );
            }
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFBusModule::OnClientDisconnectMaster )
    {
        if ( net_data->_name == KFGlobal::Instance()->_app_name &&
                net_data->_type == __STRING__( master ) )
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
        auto listen_data = &kfmsg->listen();
        if ( !CheckNeedConnection( listen_data->appname(), listen_data->apptype(), listen_data->appid() ) )
        {
            return;
        }

        _kf_tcp_client->StartClient( listen_data->appname(), listen_data->apptype(), listen_data->appid(), listen_data->ip(), listen_data->port() );
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
    bool KFBusModule::CheckNeedConnection( const std::string& connect_name, const std::string& connect_type, uint64 connect_id )
    {
        auto global = KFGlobal::Instance();

        // 不是同一类服务器
        // 如果是master, 返回false, 因为master会主动连, 不需要再这里再次连接
        // 自己不能连接自己
        if ( connect_name != global->_app_name ||
                connect_type == __STRING__( master ) ||
                connect_id == global->_app_id->GetId() )
        {
            return false;
        }

        auto str_connect_id = KFAppId::ToString( connect_id );
        for ( auto& iter : KFBusConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            if ( setting->_app_name != _globbing_string && setting->_app_name != global->_app_name )
            {
                continue;
            }

            if ( setting->_app_type != _globbing_string && setting->_app_type != global->_app_type )
            {
                continue;
            }

            if ( setting->_app_id != _globbing_string && setting->_app_id != global->_app_id->ToString() )
            {
                continue;
            }

            // 判断连接目标信息
            if ( setting->_connect_name != _globbing_string && setting->_connect_name != connect_name )
            {
                continue;
            }

            if ( setting->_connect_type != _globbing_string && setting->_connect_type != connect_type )
            {
                continue;
            }

            if ( setting->_connect_id != _globbing_string && setting->_connect_id != str_connect_id )
            {
                continue;
            }

            // 连接间隔
            if ( setting->_interval != _invalid_int && setting->_multi != _invalid_int )
            {
                KFAppId app_id( connect_id );
                auto connect_work_id = app_id.GetWorkId();
                auto work_id = ( global->_app_id->GetWorkId() + setting->_multi - 1 ) / setting->_multi;
                if ( ( __MAX__( work_id, connect_work_id ) ) - ( __MIN__( work_id, connect_work_id ) ) > setting->_interval )
                {
                    return false;
                }
            }

            return true;
        }

        return false;
    }

    std::shared_ptr<const KFBusSetting> KFBusModule::FindMasterConnection( const std::string& app_name, const std::string& app_type, const std::string& app_id )
    {
        for ( auto& iter : KFBusConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            if ( setting->_app_name != _globbing_string && setting->_app_name != app_name )
            {
                continue;
            }

            if ( setting->_app_type != _globbing_string && setting->_app_type != app_type )
            {
                continue;
            }

            if ( setting->_app_id != _globbing_string && setting->_app_id != app_id )
            {
                continue;
            }

            if ( setting->_connect_name != _globbing_string && setting->_app_name != app_name )
            {
                continue;
            }

            if ( setting->_connect_type != _globbing_string && setting->_connect_type != __STRING__( master ) )
            {
                continue;
            }

            return setting;
        }

        return nullptr;
    }
}
