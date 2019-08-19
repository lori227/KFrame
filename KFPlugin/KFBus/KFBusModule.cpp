#include "KFBusModule.hpp"
#include "KFBusConfig.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFBusModule::InitModule()
    {
        __KF_ADD_CONFIG__( KFBusConfig );
    }

    void KFBusModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_REGISTER_TO_SERVER, &KFBusModule::HanldeTellRegisterToServer );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_UNREGISTER_FROM_SERVER, &KFBusModule::HanldeTellUnRegisterFromServer );
    }

    void KFBusModule::ShutDown()
    {
        __UN_MESSAGE__( KFMsg::S2S_TELL_REGISTER_TO_SERVER );
        __UN_MESSAGE__( KFMsg::S2S_TELL_UNREGISTER_FROM_SERVER );
    }

    void KFBusModule::PrepareRun()
    {
        // 如果是master, 不执行
        auto kfglogal = KFGlobal::Instance();
        if ( kfglogal->_app_type == __KF_STRING__( master ) )
        {
            return;
        }

        // 查找需要连接的客户端, 自动连接
        IpAddressList connectionlist;
        FindConnection( connectionlist );

        for ( auto kfaddress : connectionlist )
        {
            // 只连相同小区
            KFAppId kfappid( kfaddress->_id );
            if ( kfglogal->_app_id->GetZoneId() != kfappid.GetZoneId() )
            {
                continue;
            }

            auto port = _kf_ip_address->CalcListenPort( kfaddress->_port_type, kfaddress->_port, kfaddress->_id );
            _kf_tcp_client->StartClient( kfaddress->_name, kfaddress->_type, kfaddress->_id, kfaddress->_ip, port );
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
                connecttype == __KF_STRING__( master ) ||
                connectid == kfglobal->_app_id->GetId() )
        {
            return false;
        }

        auto strconnectid = KFAppId::ToString( connectid );
        return KFBusConfig::Instance()->IsValidConnection( connectname, connecttype, strconnectid );
    }

    void KFBusModule::FindConnection( IpAddressList& outlist )
    {
        outlist.clear();

        auto kfglobal = KFGlobal::Instance();
        std::set< KFConnection* > connectionlist;
        KFBusConfig::Instance()->FindConnection( kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id->ToString(), connectionlist );

        // 查找连接的ip地址
        for ( auto kfconnection : connectionlist )
        {
            // 查找连接信息
            auto kfaddress = _kf_ip_address->FindIpAddress( kfconnection->_connect_name, kfconnection->_connect_type, kfconnection->_connect_id );
            if ( kfaddress != nullptr )
            {
                outlist.push_back( kfaddress );
            }
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

        if ( !IsConnection( kfmsg.appname(), kfmsg.apptype(), kfmsg.appid() ) )
        {
            return;
        }

        _kf_tcp_client->CloseClient( kfmsg.appid(), __FUNC_LINE__ );
    }
}
