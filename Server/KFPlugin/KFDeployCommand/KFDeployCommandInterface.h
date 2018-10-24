#ifndef __KF_DEPLOY_COMMAND_INTERFACE_H__
#define __KF_DEPLOY_COMMAND_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    // 命令回调函数
    typedef std::function<void( const std::string& param )> KFCommandFunction;

    class KFDeployCommandInterface : public KFModule
    {
    public:
        template< class T >
        void RegisterCommandFunction( const std::string& command, T* object, void( T::*handle )( const std::string& param ) )
        {
            KFCommandFunction function = std::bind( handle, object, std::placeholders::_1 );
            AddCommandFunction( command, typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterCommandFunction( const std::string& command, T* object )
        {
            RemoveComandFunction( command, typeid( T ).name() );
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // 部署命令
        virtual void DeployCommand( const std::string& command, const std::string& value, uint32 appchannel,
                                    const std::string& appname, const std::string& apptype, const std::string& appid, uint32 zoneid ) = 0;

    protected:
        virtual void AddCommandFunction( const std::string& command, const std::string& module, KFCommandFunction& function ) = 0;
        virtual void RemoveComandFunction( const std::string& command, const std::string& module ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_command, KFDeployCommandInterface );
    //////////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_COMMAND_FUNCTION__( function ) \
    void function( const std::string& param )

#define __REGISTER_COMMAND_FUNCTION__( command, function ) \
    _kf_command->RegisterCommandFunction( command, this, function )

#define __UNREGISTER_COMMAND_FUNCTION__( command ) \
    _kf_command->UnRegisterCommandFunction( command, this )

}



#endif