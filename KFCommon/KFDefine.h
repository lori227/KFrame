#ifndef __KF_DEFINE_H__
#define __KF_DEFINE_H__

#include "stdarg.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <array>
#include <algorithm>
#include <math.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <iomanip>
#include <algorithm>
#include <time.h>
#include <mutex>
#include <atomic>
#include <cctype>
#include <thread>
#include <chrono>
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#include "KFMacros.h"

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
    class PBShowElements;
    class ListenData;
}

namespace KFrame
{
    typedef void*	voidptr;
    typedef char	int8;
    typedef int16_t	int16;
    typedef int32_t	int32;
    typedef int64_t	int64;

    typedef unsigned char  uint8;
    typedef uint16_t uint16;
    typedef uint32_t uint32;
    typedef uint64_t uint64;

    /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    static const uint64 _invalid_int = 0u;
    static const double _invalid_double = 0;
    static const double _default_multiple = 1.0;
    static const std::string _invalid_string = "";
    static const std::string _globbing_string = "*";
    static const std::string _split_string = ",";
    static const std::string _range_string = "-";
    static const std::string _domain_string = ":";
    static const std::string _and_string = "&";
    static const std::string _or_string = "|";
    /////////////////////////////////////////////////////////////////////////
    typedef std::set<std::string> StringSet;
    typedef std::list<std::string> StringList;
    typedef std::vector<std::string> StringVector;

    typedef std::unordered_map< std::string, std::string > StringMap;
    typedef std::pair< std::string, std::string > StringPair;

    typedef std::list< StringMap > StringMapList;
    typedef std::list< StringPair > StringPairList;

    typedef std::set<uint32> UInt32Set;
    typedef std::list<uint32> UInt32List;
    typedef std::vector<uint32> UInt32Vector;

    typedef std::set<uint64> UInt64Set;
    typedef std::list<uint64> UInt64List;
    typedef std::vector<uint64> UInt64Vector;

    typedef std::unordered_map< uint32, uint32 > UInt32Map;
    typedef std::unordered_map< uint64, uint64 > UInt64Map;

    typedef std::unordered_map< std::string, uint32 > StringUInt32;
    typedef std::unordered_map< std::string, uint64 > StringUInt64;
    typedef std::unordered_map< uint32, std::string > UInt32String;
    typedef std::unordered_map< uint64, std::string > UInt64String;
    /////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    class Route;
    class KFData;
    class KFEntity;
    class KFElements;
    class KFElementData;

    typedef std::shared_ptr<KFData> DataPtr;
    typedef std::shared_ptr<KFEntity> EntityPtr;
    //////////////////////////////////////////////////////////////////////////
    typedef std::thread::id ThreadId;

    // 线程函数
    typedef std::function< void() > KFThreadFunction;
    //////////////////////////////////////////////////////////////////////////

    // 消息函数
    // 参数1 : Route 路由信息
    // 参数2 : msg_id 消息id
    // 参数3 : data 消息数据
    // 参数4 : length 消息数据长度
    typedef std::function<void( const Route&, uint32, const char*, uint32 )> KFMessageFunction;
    typedef std::function<bool( const Route&, uint32, const char*, uint32 )> KFForwardFunction;

    // http client 回调函数
    // 参数1 : data  发送的数据
    // 参数2 : result 返回的数据
    // 参数3 : callback 回调回传数据
    typedef std::function<void( std::string&, std::string&, std::string& )> KFHttpClientFunction;

    // http handle 函数
    // 参数1 : ip 请求者ip
    // 参数2 : data 请求的数据
    typedef std::function< std::string( const std::string&, const std::string& )> KFHttpMethodFunction;

    // 集群连接
    // 参数1 : 集群app id
    typedef std::function< void( uint64 ) > KFClusterConnectionFunction;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // 数据格式化
    // 参数1 : 配置字串
    typedef std::function< const std::string&( const std::string& ) > KFParseToElementStringFunction;

    // 参数 : element data数据
    typedef std::function< const std::string&( const KFElementData& ) > KFDataToElementStringFunction;

    // 参数1 : 属性名
    // 参数2 : 属性数值(固定值)
    // 参数3 : 属性id
    typedef std::function< const std::string&( const std::string&, uint32, uint32 ) > KFIntToElementStringFunction;

    // 参数1 : 属性名
    // 参数2 : 属性数值(可以是一个范围 如:"1-100")
    // 参数3 : 属性id
    typedef std::function< const std::string&( const std::string&, const std::string&, uint32 ) > KFStrToElementStringFunction;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_INTERFACE__( attribute, classname )	\
    class I##classname : public KFSingleton< I##classname >	\
    {\
    public:\
        classname* _interface = nullptr; \
    }; \
    static classname*& attribute = I##classname::Instance()->_interface;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif // !__KF_DEFINE_H__
