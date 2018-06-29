#ifndef __KF_COMMAND_INTERFACE_H__
#define __KF_COMMAND_INTERFACE_H__

#include "KFrame.h"
#include "KFInterfaces.h"

namespace KFrame
{

    typedef std::function< void( KFJson& json ) > KFCommandFunction;

    class KFCommandInterface : public KFModule
    {
    public:
        // 添加命令
        virtual void AddCommand( const char* appname, uint32 appid, const char* command ) = 0;

        // 注册处理函数
        template< class T >
        void RegisterCommandFunction( const char* command, T* object, void ( T::*handle )( KFJson& json ) )
        {
            KFCommandFunction function = std::bind( handle, object, std::placeholders::_1 );

            RegisterFunction( command, function );
        }

        // 卸载
        virtual void UnRegisterCommandFunction( const char* command ) = 0;

    protected:
        // 注册函数
        virtual void RegisterFunction( const char* command, KFCommandFunction& function ) = 0;
    };


    //////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_command, KFCommandInterface );
    //////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif