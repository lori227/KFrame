#ifndef __KF_HTTP_CLIENT_INTERFACE_H__
#define __KF_HTTP_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    // ST 单线程http请求
    // MT 多线程http请求
    class KFHttpClientInterface : public KFModule
    {
    public:
        // 单线程
        virtual std::string STGet( const std::string& url, KFJson& json ) = 0;
        virtual std::string STGet( const std::string& url, const std::string& data ) = 0;

        virtual std::string STPost( const std::string& url, KFJson& json ) = 0;
        virtual std::string STPost( const std::string& url, const std::string& data ) = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 多线程 string
        template< class T >
        void MTGet( const std::string& url, const std::string& data,
                    T* object = nullptr, void ( T::*handle )( std::string&, std::string&, std::string& ) = nullptr,
                    const std::string& args = _invalid_string )
        {
            KFHttpClientFunction function = nullptr;
            if ( object != nullptr )
            {
                function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            }

            MTGetRequest( function, url, data, args );
        }

        template< class T >
        void MTPost( const std::string& url, const std::string& data,
                     T* object = nullptr, void ( T::*handle )( std::string&, std::string&, std::string& ) = nullptr,
                     const std::string& args = _invalid_string )
        {
            KFHttpClientFunction function = nullptr;
            if ( object != nullptr )
            {
                function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            }

            MTPostRequest( function, url, data, args );
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 多线程 json
        template< class T >
        void MTGet( const std::string& url, KFJson& json,
                    T* object = nullptr, void ( T::*handle )( std::string&, std::string&, std::string& ) = nullptr,
                    const std::string& args = _invalid_string )
        {
            KFHttpClientFunction function = nullptr;
            if ( object != nullptr )
            {
                function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            }

            MTGetRequest( function, url, json, args );
        }

        template< class T >
        void MTPost( const std::string& url, KFJson& json,
                     T* object = nullptr, void ( T::*handle )( std::string&, std::string&, std::string& ) = nullptr,
                     const std::string& args = _invalid_string )
        {
            KFHttpClientFunction function = nullptr;
            if ( object != nullptr )
            {
                function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            }

            MTPostRequest( function, url, json, args );
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送错误
        virtual std::string SendCode( uint32 code ) = 0;

        // 获得错误码
        virtual uint32 GetCode( KFJson& json ) = 0;

        // 发送json
        virtual std::string SendResponse( KFJson& json ) = 0;

    protected:
        virtual void MTGetRequest( KFHttpClientFunction& function, const std::string& url, const std::string& data, const std::string& args ) = 0;
        virtual void MTPostRequest( KFHttpClientFunction& function, const std::string& url, const std::string& data, const std::string& args ) = 0;

        virtual void MTGetRequest( KFHttpClientFunction& function, const std::string& url, KFJson& json, const std::string& args ) = 0;
        virtual void MTPostRequest( KFHttpClientFunction& function, const std::string& url, KFJson& json, const std::string& args ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_http_client, KFHttpClientInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

#define __KF_HTTP_CALL_BACK_FUNCTION__( function )\
    void function( std::string& senddata, std::string& recvdata, std::string& args )

}



#endif