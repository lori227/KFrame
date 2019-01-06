#ifndef __KF_DEFINE_H__
#define __KF_DEFINE_H__

#include "KFId.h"

namespace google
{
    namespace protobuf
    {
        class Message;
    }
}

namespace KFMsg
{
    class PBObject;
}

namespace KFrame
{
    ////////////////////////////////////////////////////////////////
    static const uint32 _invalid_int = 0;
    static const std::string _invalid_str = "";
    static const std::string _globbing_str = "*";
    /////////////////////////////////////////////////////////////////////////
    class KFIpAddress;
    typedef std::set< KFIpAddress* > IpAddressList;

    typedef std::list< std::string > ListString;
    typedef std::vector< std::string > VectorString;
    typedef std::map< std::string, std::string > MapString;
    /////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////
    // 网络函数
    typedef std::function<void( const KFId& kfid, const char* data, uint32 length )> KFMessageFunction;
    typedef std::function<void( const KFId& kfid, uint32 msgid, const char* data, uint32 length )> KFNetFunction;
    typedef std::function<bool( const KFId& kfid, uint32 msgid, const char* data, uint32 length )> KFTransmitFunction;
    ////////////////////////////////////////////////////////////////////////
    // 客户端 连接 和 断开 回调函数
    typedef std::function< void( uint64 serverid, const std::string& servername, const std::string& servertype ) > KFClientLostFunction;
    typedef std::function< void( uint64 serverid, const std::string& servername, const std::string& servertype ) > KFClientConnectionFunction;
    ////////////////////////////////////////////////////////////////////////
    // 服务器 连接 和 断开 回调函数
    typedef std::function< void( uint64 id, const std::string& name, const std::string& type ) > KFServerLostFunction;
    typedef std::function< void( uint64 id, const std::string& name, const std::string& type, const std::string& ip, uint32 port ) > KFServerDiscoverFunction;
    ////////////////////////////////////////////////////////////////////////
    // http client 回调函数
    typedef std::function<void( std::string& data, std::string& result, std::string& callback )> KFHttpClientFunction;

    // httphandle 函数
    typedef std::function< std::string ( const std::string& ip, const std::string& data )> KFHttpMethodFunction;

    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    // 日志函数
    typedef std::function< bool( uint32, const std::string& ) > KFLogFunction;

}

#endif // !__KF_DEFINE_H__
