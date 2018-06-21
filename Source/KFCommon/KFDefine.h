#ifndef __KF_DEFINE_H__
#define __KF_DEFINE_H__

#include "KFMap.h"
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

namespace Json
{
	class Value;
}

namespace Math3D
{
	class Vector3D;
}

namespace KFrame
{
#ifndef __MIN__
#define __MIN__( x, y ) ( (x) > (y) ? (y) : (x) )
#endif

#ifndef __MAX__
#define __MAX__( x, y ) ( (x) > (y) ? (x) : (y) )
#endif

#ifndef __DELETE_ARRAY__
#define __DELETE_ARRAY__( p ) if ( p != nullptr ) { delete[] p; p = nullptr; }
#endif

#define __KF_STRING__( value ) std::to_string( value )
	////////////////////////////////////////////////////////////////
	struct StringLesser : std::binary_function<std::string, std::string, bool >
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

	typedef std::list< std::string > ListString;
	typedef std::vector< std::string > VectorString;
	typedef std::map< std::string, std::string > MapString;
	typedef std::map<std::string, std::string, StringLesser > LesserMapString;
	/////////////////////////////////////////////////////////////////////////
	static const uint32 _invalid_int = 0;
	static const std::string _invalid_str = "";
	static ListString _invalid_list_str;
	/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	// 网络函数
	typedef std::function<void( const KFGuid& guid, const char* data, uint32 length )> KFMessageFunction;
	typedef std::function<void( const KFGuid& guid, uint32 msgid, const char* data, uint32 length )> KFNetFunction;
	typedef std::function<bool( const KFGuid& guid, uint32 msgid, const char* data, uint32 length )> KFTransmitFunction;
#define __KF_MESSAGE_FUNCTION__( function ) void function( const KFGuid& kfguid, const char* data, uint32 length )
#define __KF_TRANSMIT_FUNCTION__( function ) bool function( const KFGuid& kfguid, uint32 msgid, const char* data, uint32 length )
	////////////////////////////////////////////////////////////////////////
	// 客户端 连接 和 断开 回调函数
	typedef std::function< void( uint32 serverid, const std::string& servername, const std::string& servertype ) > KFClientLostFunction;
	typedef std::function< void( uint32 serverid, const std::string& servername, const std::string& servertype ) > KFClientConnectionFunction;
#define __KF_CLIENT_LOST_FUNCTION__( function ) void function( uint32 serverid, const std::string& servername, const std::string& servertype )
#define __KF_CLIENT_CONNECT_FUNCTION__( function ) void function( uint32 serverid, const std::string& servername, const std::string& servertype )
	////////////////////////////////////////////////////////////////////////
	// 服务器 连接 和 断开 回调函数
	typedef std::function< void( uint32 id, const std::string& name, const std::string& type ) > KFServerLostFunction;
	typedef std::function< void( uint32 id, const std::string& name, const std::string& type, const std::string& ip, uint32 port ) > KFServerDiscoverFunction;

#define __KF_SERVER_LOST_FUNCTION__( function ) void function( uint32 handleid, const std::string& handlename, const std::string& handletype )
#define __KF_SERVER_DISCOVER_FUNCTION__( function ) void function( uint32 handleid, const std::string& handlename , const std::string& handletype, const std::string& ip, uint32 port )
	////////////////////////////////////////////////////////////////////////
	// httpclient回调函数
	typedef std::function<void( std::string& data, std::string& result )> KFHttpClientFunction;

	// httphandle 函数
	typedef std::function< std::string ( const std::string& ip, const std::string& data )> KFHttpMethodFunction;

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	typedef std::function< void() > KFThreadFunction;
	
	//////////////////////////////////////////////////////////////////////////////
	// 连接定义
	class KFConnection
	{
	public:
		KFConnection()
		{
			_id = 0;
			_port = 0;
			_timeout = 2000;
		}

		inline bool IsValid() const
		{
			return !_ip.empty();
		}

	public:

		// 服务器类型
		std::string _type;

		// 服务器id
		uint32 _id;

		// 服务器名字
		std::string _name;

		// 服务器ip
		std::string _ip;

		// 服务器端口
		int32 _port;

		// 连接超时时间
		uint32 _timeout;

		// 集群的认证key
		std::string _key;
	};
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
}





#endif // !__KF_DEFINE_H__
