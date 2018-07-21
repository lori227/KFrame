#ifndef __KF_DEFINE_H__
#define __KF_DEFINE_H__

#include "KFGuid.h"

namespace google
{
    namespace protobuf
    {
        class Message;
    }
}

namespace KFMsg
{
    class ListenData;

    class PBInt32;
    class PBUInt32;
    class PBInt64;
    class PBUInt64;
    class PBDouble;
    class PBString;
    class PBStrings;
    class PBVector3D;
    class PBObject;
    class PBRecord;
    class PBArray;
}

namespace Poco
{
    class URI;
    namespace Net
    {
        class HTTPRequest;
        class HTTPClientSession;
    }
}
namespace Math3D
{
    class Vector3D;
}

namespace KFrame
{

    ////////////////////////////////////////////////////////////////
    struct StringGreater : std::binary_function<std::string, std::string, bool >
    {
        bool operator() ( const std::string& x, const std::string& y ) const
        {
            if ( x.length() > y.length() )
            {
                return true;
            }

            if ( x.length() < y.length() )
            {
                return false;
            }

            return ( x > y ? true : false );
        }
    };

    class KFJson;

    class KFIpAddress;
    typedef std::set< KFIpAddress* > IpAddressList;

    typedef std::list< std::string > ListString;
    typedef std::vector< std::string > VectorString;
    typedef std::map< std::string, std::string > MapString;
    typedef std::map<std::string, std::string, StringGreater > GreaterMapString;
    /////////////////////////////////////////////////////////////////////////
    static const uint32 _invalid_int = 0;
    static const std::string _invalid_str = "";
    static const std::string _globbing_str = "*";
    static ListString _invalid_list_str;
    /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    // 网络函数
    typedef std::function<void( const KFGuid& guid, const char* data, uint32 length )> KFMessageFunction;
    typedef std::function<void( const KFGuid& guid, uint32 msgid, const char* data, uint32 length )> KFNetFunction;
    typedef std::function<bool( const KFGuid& guid, uint32 msgid, const char* data, uint32 length )> KFTransmitFunction;
    ////////////////////////////////////////////////////////////////////////
    // 客户端 连接 和 断开 回调函数
    typedef std::function< void( uint32 serverid, const std::string& servername, const std::string& servertype ) > KFClientLostFunction;
    typedef std::function< void( uint32 serverid, const std::string& servername, const std::string& servertype ) > KFClientConnectionFunction;
    ////////////////////////////////////////////////////////////////////////
    // 服务器 连接 和 断开 回调函数
    typedef std::function< void( uint32 id, const std::string& name, const std::string& type ) > KFServerLostFunction;
    typedef std::function< void( uint32 id, const std::string& name, const std::string& type, const std::string& ip, uint32 port ) > KFServerDiscoverFunction;
    ////////////////////////////////////////////////////////////////////////
    // httpclient回调函数
    typedef std::function<void( std::string& data, std::string& result )> KFHttpClientFunction;

    // httphandle 函数
    typedef std::function< std::string ( const std::string& ip, const std::string& data )> KFHttpMethodFunction;

    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
}





#endif // !__KF_DEFINE_H__
