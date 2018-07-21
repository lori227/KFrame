#include "KFBusModule.h"
#include "KFBusConfig.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFBusModule::KFBusModule()
    {
    }

    KFBusModule::~KFBusModule()
    {

    }

    void KFBusModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_bus_config, true );
    }

    void KFBusModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_REGISTER_TO_SERVER, &KFBusModule::HanldeTellRegisterToServer );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_UNREGISTER_FROM_SERVER, &KFBusModule::HanldeTellUnRegisterFromServer );
    }

    void KFBusModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__();

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
            auto port = _kf_ip_address->CalcTcpListenPort( kfaddress->_port_type, kfaddress->_port, kfaddress->_app_id );
            _kf_tcp_client->StartClient( kfaddress->_app_name, kfaddress->_app_type, kfaddress->_app_id, kfaddress->_ip, port );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFBusModule::IsConnection( const std::string& connectname, const std::string& connecttype, uint32 connectid )
    {
        auto kfglobal = KFGlobal::Instance();

        // 如果是master, 返回false, 因为master会主动连, 不需要再这里再次连接
        if ( connecttype == KFField::_master )
        {
            return false;
        }

        auto ok = _kf_bus_config->IsValidConnection( kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id,
                  connectname, connecttype, connectid );

        return ok;
    }

    void KFBusModule::FindConnection( IpAddressList& outlist )
    {
        outlist.clear();

        auto kfglobal = KFGlobal::Instance();
        std::set< KFConnection* > connectionlist;
        _kf_bus_config->FindConnection( kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id, connectionlist );

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
        _kf_tcp_client->CloseClient( kfmsg.appid() );
    }
}
