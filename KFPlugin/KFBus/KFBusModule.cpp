#include "KFBusModule.hpp"
#include "KFBusConfig.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFBusModule::BeforeRun()
    {
        __REGISTER_CLIENT_FAILED__( &KFBusModule::OnClientConnectMasterFailed );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_REGISTER_TO_SERVER, &KFBusModule::HanldeTellRegisterToServer );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_UNREGISTER_FROM_SERVER, &KFBusModule::HanldeTellUnRegisterFromServer );
    }

    void KFBusModule::ShutDown()
    {
        __UN_CLIENT_FAILED__();
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
        auto kfconnection = KFBusConfig::Instance()->FindMasterConnection( kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id->ToString() );
        if ( kfconnection == nullptr )
        {
            return;
        }

        // 启动定时器
        __LOOP_TIMER_0__( 6000u, 1000u, &KFBusModule::OnTimerConnectionMaster );
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
        auto kfglobal = KFGlobal::Instance();
        if ( netdata->_name == kfglobal->_app_name && netdata->_type == __STRING__( master ) )
        {
            ++_connect_master_failed_count;
            if ( _connect_master_failed_count > 5u )
            {
                // 超过设定次数, 重新连接
                _kf_tcp_client->CloseClient( netdata->_id, __FUNC_LINE__ );

                // 启动定时器
                __LOOP_TIMER_0__( 6000u, 1000u, &KFBusModule::OnTimerConnectionMaster );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFBusModule::IsConnection( const std::string& connectname, const std::string& connecttype, uint64 connectid )
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
        return KFBusConfig::Instance()->IsValidConnection( connectname, connecttype, strconnectid );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 通知有客户端注册
    __KF_MESSAGE_FUNCTION__( KFBusModule::HanldeTellRegisterToServer )
    {
        __PROTO_PARSE__( KFMsg::TellRegisterToServer );

        auto listendata = &kfmsg.listen();
        if ( !IsConnection( listendata->appname(), listendata->apptype(), listendata->appid() ) )
        {
            return;
        }

        _kf_tcp_client->StartClient( listendata->appname(), listendata->apptype(), listendata->appid(), listendata->ip(), listendata->port() );
    }

    __KF_MESSAGE_FUNCTION__( KFBusModule::HanldeTellUnRegisterFromServer )
    {
        __PROTO_PARSE__( KFMsg::TellUnRegisterFromServer );

        if ( !IsConnection( kfmsg.appname(), kfmsg.apptype(), kfmsg.appid() ) )
        {
            return;
        }

        _kf_tcp_client->CloseClient( kfmsg.appid(), __FUNC_LINE__ );
    }
}
