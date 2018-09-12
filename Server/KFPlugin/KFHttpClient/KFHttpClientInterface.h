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
        virtual std::string StartSTHttpsClient( const std::string& url, const std::string& data ) = 0;

        virtual std::string StartSTHttpClient( const std::string& url, KFJson& json, bool sign ) = 0;
        virtual std::string StartSTHttpsClient( const std::string& url, KFJson& json, bool sign ) = 0;

        // 多线程
        virtual void StartMTHttpClient( const std::string& url, const std::string& data ) = 0;
        virtual void StartMTHttpsClient( const std::string& url, const std::string& data ) = 0;

        virtual void StartMTHttpClient( const std::string& url, KFJson& json, bool sign ) = 0;
        virtual void StartMTHttpsClient( const std::string& url, KFJson& json, bool sign ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class T >
        void StartMTHttpClient( const std::string& url, const std::string& data, T* object, void ( T::*handle )( std::string&, std::string& ) )
        {
            KFHttpClientFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            StartMTHttpClient( url, data, function );
        }

        template< class T >
        void StartMTHttpsClient( const std::string& url, const std::string& data, T* object, void ( T::*handle )( std::string&, std::string& ) )
        {
            KFHttpClientFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            StartMTHttpsClient( url, data, function );
        }

        template< class T >
        void StartMTHttpClient( const std::string& url, KFJson& json, bool sign,  T* object, void ( T::*handle )( std::string&, std::string& ) )
        {
            KFHttpClientFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            StartMTHttpClient( url, json, function, sign );
        }

        template< class T >
        void StartMTHttpsClient( const std::string& url, KFJson& json, bool sign, T* object, void ( T::*handle )( std::string&, std::string& ) )
        {
            KFHttpClientFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            StartMTHttpsClient( url, json, function, sign );
        }

        /////////////////////////////////////////////////////////////////////////

        // 返回错误
        virtual std::string SendResponseCode( uint32 code ) = 0;

        // 获得错误码
        virtual uint32 GetResponseCode( KFJson& json ) = 0;

        // 发送json
        virtual std::string SendResponse( KFJson& json ) = 0;

        // 创建签名
        virtual void MakeSignature( KFJson& json ) = 0;

        // 验证签名
        virtual bool VerifySignature( KFJson& json ) = 0;

    protected:

        // http
        virtual void StartMTHttpClient( const std::string& url, const std::string& data, KFHttpClientFunction& function ) = 0;
        virtual void StartMTHttpClient( const std::string& url, KFJson& json, KFHttpClientFunction& function, bool sign ) = 0;

        // https
        virtual void StartMTHttpsClient( const std::string& url, const std::string& data, KFHttpClientFunction& function ) = 0;
        virtual void StartMTHttpsClient( const std::string& url, KFJson& json, KFHttpClientFunction& function, bool sign ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_http_client, KFHttpClientInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif