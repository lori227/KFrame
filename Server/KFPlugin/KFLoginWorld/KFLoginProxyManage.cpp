#include "KFLoginProxyManage.h"

namespace KFrame
{
    KFLoginProxyManage::KFLoginProxyManage()
    {

    }

    KFLoginProxyManage::~KFLoginProxyManage()
    {

    }

    KFLoginProxy* KFLoginProxyManage::AddLoginProxy( uint32 handleid, const std::string& ip, uint32 port )
    {
        auto logingate = _kf_proxy_list.Create( handleid );
        logingate->_ip = ip;
        logingate->_port = port;
        logingate->_proxy_id = handleid;

        return logingate;
    }

    void KFLoginProxyManage::UpdateLoginProxy( uint32 handleid, uint32 clientcount, const std::string& ip, uint32 port )
    {
        auto loginproxy = AddLoginProxy( handleid, ip, port );
        loginproxy->_client_count = clientcount;
    }

    void KFLoginProxyManage::RemoveLoginProxy( uint32 handleid )
    {
        _kf_proxy_list.Remove( handleid );
    }

    KFLoginProxy* KFLoginProxyManage::FindLoginProxy( uint32 handleid )
    {
        return _kf_proxy_list.Find( handleid );
    }

    KFLoginProxy* KFLoginProxyManage::SelectLoginProxy()
    {
        auto maxclientcount = 0xFFFFFFFF;
        KFLoginProxy* loginproxy = nullptr;

        for ( auto& iter : _kf_proxy_list._objects )
        {
            auto temp = iter.second;

            if ( temp->_client_count < maxclientcount )
            {
                maxclientcount = temp->_client_count;
                loginproxy = temp;
            }
        }

        // 先加一个连接数量, 必选同一时间都选择同有一个服务器
        if ( loginproxy != nullptr )
        {
            ++loginproxy->_client_count;
        }

        return loginproxy;
    }
}