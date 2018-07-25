#ifndef __KF_DEPLOY_COMMAND_INTERFACE_H__
#define __KF_DEPLOY_COMMAND_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    // 命令回调函数
    typedef std::function<void()> KFCommandFunction;

    class KFDeployCommandInterface : public KFModule
    {
    public:

        template< class T >
        void RegisterShutDownFunction( T* object, void( T::*handle )( ) )
        {
            KFCommandFunction function = std::bind( handle, object );
            AddShutDownFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterShutDownFunction( T* object )
        {
            RemoveShutDownFunction( typeid( T ).name() );
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // 关闭服务
        virtual void ShutDownServer( const std::string& appname, const std::string& apptype, uint32 appid, uint32 zoneid, uint32 delaytime ) = 0;

    protected:
        virtual void AddShutDownFunction( const std::string& module, KFCommandFunction& function ) = 0;
        virtual void RemoveShutDownFunction( const std::string& module ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_deploy_command, KFDeployCommandInterface );
    //////////////////////////////////////////////////////////////////////////////////////////////////

#define __KF_COMMAND_FUNCTION__( function ) \
    void function( )

#define __REGISTER_SHUTDOWN_FUNCTION__( function ) \
    _kf_deploy_command->RegisterShutDownFunction( this, function )

#define __UNREGISTER_SHUTDOWN_FUNCTION__( ) \
    _kf_deploy_command->UnRegisterShutDownFunction( this )

}



#endif