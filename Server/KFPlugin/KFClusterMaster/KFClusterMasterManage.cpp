#include "KFClusterMasterManage.h"

namespace KFrame
{

    void KFProxyManage::AddProxyServer( const std::string& type, uint32 handleid, const std::string& name, const std::string& ip, uint32 port, uint32 clientcount )
    {
        auto kfgate = _kf_proxy_list.Find( handleid );
        if ( kfgate == nullptr )
        {
            kfgate = __KF_CREATE__( KFProxyData );
            kfgate->_id = handleid;
            kfgate->_type = type;
            kfgate->_name = name;
            kfgate->_ip = ip;
            kfgate->_port = port;
            _kf_proxy_list.Insert( handleid, kfgate );
            _kf_hash.AddHashNode( name, handleid, 100 );
        }

        kfgate->_client_count = clientcount;
    }

    void KFProxyManage::UpdateProxyServer( uint32 handleid, uint32 clientcount )
    {
        auto kfgate = _kf_proxy_list.Find( handleid );
        if ( kfgate == nullptr )
        {
            return;
        }

        kfgate->_client_count = clientcount;
    }

    void KFProxyManage::RemoveProxyServer( uint32 handleid )
    {
        _kf_proxy_list.Remove( handleid );
        _kf_hash.RemoveHashNode( handleid );
    }

    KFProxyData* KFProxyManage::FindProxyServer( uint32 handleid )
    {
        return _kf_proxy_list.Find( handleid );
    }

    KFProxyData* KFProxyManage::SelectProxyServer()
    {
        auto maxclientcount = 0xFFFFFFFF;
        KFProxyData* kfproxydata = nullptr;

        for ( auto& iter : _kf_proxy_list._objects )
        {
            auto temp = iter.second;

            if ( temp->_client_count < maxclientcount )
            {
                maxclientcount = temp->_client_count;
                kfproxydata = temp;
            }
        }

        // 先加一个连接数量, 必选同一时间都选择同有一个服务器
        if ( kfproxydata != nullptr )
        {
            ++kfproxydata->_client_count;
        }

        return kfproxydata;
    }

    KFProxyData* KFProxyManage::HashProxyServer( uint32 clientid )
    {
        auto proxyid = _kf_hash.FindHashNode( clientid, true );
        if ( proxyid == _invalid_int )
        {
            return nullptr;
        }

        return FindProxyServer( proxyid );
    }
}