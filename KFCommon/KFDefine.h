#ifndef __KF_DEFINE_H__
#define __KF_DEFINE_H__

#include "KFRouter.h"

namespace google
{
    namespace protobuf
    {
        class Message;

        template <typename Key, typename T>
        class Map;
    }
}

namespace KFMsg
{
    class PBObject;
    class PBRecord;
    class PBLoginData;
    class PBShowElement;
    class ListenData;
}

namespace KFrame
{
    class KFData;
    class KFEntity;
    ////////////////////////////////////////////////////////////////
    static const uint64 _invalid_int = 0u;
    static const std::string _invalid_str = "";
    static const std::string _globbing_str = "*";
    static const std::string _route_cluster_name = "route";
    static const std::string _route_cluster_key = "route@kframe,./";
    /////////////////////////////////////////////////////////////////////////
    // Ip地址配置
    class KFNetData
    {
    public:
        // 名字
        std::string _name;

        //类型
        std::string _type;

        // id
        uint64 _id = 0;
        std::string _str_id;

        // 连接id
        uint64 _session = 0;

        // 服务器ip
        std::string _ip;

        // 端口类型
        uint32 _port_type = 0;

        // 服务器端口
        uint32 _port = 0;
    };

    typedef std::vector< const KFNetData* > NetDataList;

    typedef KFNetData KFIpAddress;
    typedef std::vector< const KFIpAddress* > IpAddressList;
    /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    typedef std::list< std::string > ListString;
    typedef std::vector< std::string > VectorString;
    typedef std::unordered_map< std::string, std::string > MapString;
    typedef std::list< MapString > ListMapString;
    /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    // 网络函数
    typedef std::function<void( const Route& route, const char* data, uint32 length )> KFMessageFunction;
    typedef std::function<void( const Route& route, uint32 msgid, const char* data, uint32 length )> KFNetFunction;
    typedef std::function<bool( const Route& route, uint32 msgid, const char* data, uint32 length )> KFTranspondFunction;
    ////////////////////////////////////////////////////////////////////////
    // 网络事件函数
    typedef std::function< void( const KFNetData* ) > KFNetEventFunction;
#define __KF_NET_EVENT_FUNCTION__( function )  void function( const KFNetData* netdata )
    ////////////////////////////////////////////////////////////////////////
    // http client 回调函数
    typedef std::function<void( std::string& data, std::string& result, std::string& callback )> KFHttpClientFunction;

    // httphandle 函数
    typedef std::function< std::string ( const std::string& ip, const std::string& data )> KFHttpMethodFunction;

    //////////////////////////////////////////////////////////////////////////////
    // 集群连接
    typedef std::function< void( uint64 ) > KFClusterConnectionFunction;
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////


}

#endif // !__KF_DEFINE_H__
