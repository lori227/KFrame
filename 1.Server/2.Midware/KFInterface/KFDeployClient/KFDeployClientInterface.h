#ifndef __KF_DEPLOY_CLIENT_INTERFACE_H__
#define __KF_DEPLOY_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    // 命令回调函数
    typedef std::function<void( const std::string& param )> KFCommandFunction;

    // 命令属性
    class KFCommand
    {
    public:
        KFBind< std::string, const std::string&, KFCommandFunction > _command_function;
    };

    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    class KFDeployClientInterface : public KFModule
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

    protected:
        virtual void AddCommandFunction( const std::string& command, const std::string& module, KFCommandFunction& function ) = 0;
        virtual void RemoveComandFunction( const std::string& command, const std::string& module ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_deploy_client, KFDeployClientInterface );
    ///////////////////////////////////////////////////////////////////////
#define __KF_DEPLOY_COMMAND_FUNCTION__( function ) \
    void function( const std::string& param )

#define __REGISTER_DEPLOY_COMMAND_FUNCTION__( command, function ) \
    _kf_deploy_client->RegisterCommandFunction( command, this, function )

#define __UNREGISTER_DEPLOY_COMMAND_FUNCTION__( command ) \
    _kf_deploy_client->UnRegisterCommandFunction( command, this )
}



#endif