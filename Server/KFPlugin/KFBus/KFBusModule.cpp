#include "KFBusModule.h"
#include "KFBusConfig.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFBusModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_bus_config, false );
    }

    void KFBusModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_REGISTER_TO_SERVER, &KFBusModule::HanldeTellRegisterToServer );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_UNREGISTER_FROM_SERVER, &KFBusModule::HanldeTellUnRegisterFromServer );
    }

    void KFBusModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__( _kf_bus_config );

        __UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_REGISTER_TO_SERVER );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_UNREGISTER_FROM_SERVER );
    }

    void KFBusModule::OnceRun()
    {
        // 查找需要连接的客户端, 自动连接
        IpAddressList connectionlist;
        FindConnection( connectionlist );

        for ( auto* kfaddress : connectionlist )
        {
            auto port = _kf_ip_address->CalcListenPort( kfaddress->_port_type, kfaddress->_port, kfaddress->_app_id );
            _kf_tcp_client->StartClient( kfaddress->_app_name, kfaddress->_app_type, kfaddress->_app_id, kfaddress->_ip, port );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFBusModule::IsConnection( const std::string& connectname, const std::string& connecttype, uint32 connectid, uint32 zoneid )
    {
        auto kfglobal = KFGlobal::Instance();

        // 不是同一类服务器
        // 如果是master, 返回false, 因为master会主动连, 不需要再这里再次连接
        // 如果是不同小区, 不能连接
        if ( connectname != kfglobal->_app_name ||
                connecttype == __KF_STRING__( master ) ||
                zoneid != kfglobal->_zone_id )
        {
            return false;
        }

        auto strconnectid = KFAppID::ToString( connectid );
        auto ok = _kf_bus_config->IsValidConnection( kfglobal->_app_name, kfglobal->_app_type, kfglobal->_str_app_id,
                  connectname, connecttype, strconnectid );

        return ok;
    }

    void KFBusModule::FindConnection( IpAddressList& outlist )
    {
        outlist.clear();

        auto kfglobal = KFGlobal::Instance();
        std::set< KFConnection* > connectionlist;
        _kf_bus_config->FindConnection( kfglobal->_app_name, kfglobal->_app_type, kfglobal->_str_app_id, connectionlist );

        // 查找连接的ip地址
        for ( auto* kfconnection : connectionlist )
        {
            // 查找连接信息
            _kf_ip_address->FindIpAddress( kfconnection->_connect_name, kfconnection->_connect_type, kfconnection->_connect_id, outlist );
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 通知有客户端注册
    __KF_MESSAGE_FUNCTION__( KFBusModule::HanldeTellRegisterToServer )
    {
        __PROTO_PARSE__( KFMsg::TellRegisterToServer );

        auto kfglobal = KFGlobal::Instance();
        if ( kfmsg.servername() != kfglobal->_app_name ||
                kfmsg.serverzoneid() != kfglobal->_zone_id )
        {
            return;
        }

        auto listendata = &kfmsg.listen();
        if ( !IsConnection( listendata->appname(), listendata->apptype(), listendata->appid(), listendata->zoneid() ) )
        {
            return;
        }

        _kf_tcp_client->StartClient( listendata->appname(), listendata->apptype(), listendata->appid(), listendata->ip(), listendata->port() );
    }

    __KF_MESSAGE_FUNCTION__( KFBusModule::HanldeTellUnRegisterFromServer )
    {
        __PROTO_PARSE__( KFMsg::TellUnRegisterFromServer );

        auto kfglobal = KFGlobal::Instance();
        if ( kfmsg.servername() != kfglobal->_app_name ||
                kfmsg.serverzoneid() != kfglobal->_zone_id )
        {
            return;
        }

        if ( !IsConnection( kfmsg.appname(), kfmsg.apptype(), kfmsg.appid(), kfmsg.zoneid() ) )
        {
            return;
        }

        _kf_tcp_client->CloseClient( kfmsg.appid(), __FUNC_LINE__ );
    }
}
