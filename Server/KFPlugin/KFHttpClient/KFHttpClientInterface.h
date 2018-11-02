#ifndef __KF_HTTP_CLIENT_INTERFACE_H__
#define __KF_HTTP_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFJson;

    class KFHttpClientInterface : public KFModule
    {
    public:
        // 单线程
        virtual std::string StartSTHttpClient( const std::string& url, const std::string& data ) = 0;
        virtual std::string StartSTHttpClient( const std::string& url, KFJson& json ) = 0;

        // 多线程
        virtual void StartMTHttpClient( const std::string& url, const std::string& data ) = 0;
        virtual void StartMTHttpClient( const std::string& url, KFJson& json ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class T >
        void StartMTHttpClient( T* object, void ( T::*handle )( std::string&, std::string&, std::string& ),
                                const std::string& url, const std::string& data, const std::string& callback = _invalid_str )
        {
            KFHttpClientFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            StartMTHttpClient( function, url, data,  callback );
        }

        template< class T >
        void StartMTHttpClient( T* object, void ( T::*handle )( std::string&, std::string&, std::string& ),
                                const std::string& url, KFJson& json, const std::string& callback = _invalid_str )
        {
            KFHttpClientFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            StartMTHttpClient( function, url, json, callback );
        }

        /////////////////////////////////////////////////////////////////////////
        // 返回错误
        virtual std::string SendResponseCode( uint32 code ) = 0;

        // 获得错误码
        virtual uint32 GetResponseCode( KFJson& json ) = 0;

        // 发送json
        virtual std::string SendResponse( KFJson& json ) = 0;

    protected:
        // http
        virtual void StartMTHttpClient( KFHttpClientFunction& function, const std::string& url, const std::string& data, const std::string& callback ) = 0;
        virtual void StartMTHttpClient( KFHttpClientFunction& function, const std::string& url, KFJson& json, const std::string& callback ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_http_client, KFHttpClientInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

#define __KF_HTTP_CALL_BACK_FUNCTION__( function )\
    void function( std::string& senddata, std::string& recvdata, std::string& callback )

}



#endif