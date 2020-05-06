#ifndef __KF_HTTP_SERVER_INTERFACE_H__
#define __KF_HTTP_SERVER_INTERFACE_H__

#include "KFrame.h"

// Cinatra http://purecpp.org/ httpserver框架
namespace KFrame
{
    class KFHttpServerInterface : public KFModule
    {
    public:
        // 添加服务函数
        template<typename T>
        void RegisterFunction( const std::string& url, bool sync, T* object, std::string ( T::*handle )( const std::string& ip, const std::string& data ) )
        {
            KFHttpMethodFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            RegisterMethodFunction( url, sync, function );
        }

        // 卸载
        virtual void UnRegisterFunction( const std::string& url ) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////
        // 获得完整路径
        virtual const std::string& GetHttpUrl() = 0;

        // 返回错误
        virtual std::string SendCode( uint32 code ) = 0;

        // 获得错误码
        virtual uint32 GetCode( KFJson& json ) = 0;

        // 发送json
        virtual std::string SendResponse( KFJson& json ) = 0;
        virtual std::string SendResponse( KFJson& json, uint32 code ) = 0;

    private:
        // 注册函数
        virtual void RegisterMethodFunction( const std::string& url, bool sync, KFHttpMethodFunction& function ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_http_server, KFHttpServerInterface );
    ///////////////////////////////////////////////////////////////////////////////////////////////

#define __KF_HTTP_FUNCTION__( function ) \
    std::string function( const std::string& ip, const std::string& data )

#define __REGISTER_HTTP__( url, sync, function )\
    _kf_http_server->RegisterFunction( url, sync, this, function )

#define __UN_HTTP__( url )\
    _kf_http_server->UnRegisterFunction( url )
}

#endif