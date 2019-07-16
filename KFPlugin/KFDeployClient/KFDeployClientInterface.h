#ifndef __KF_DEPLOY_CLIENT_INTERFACE_H__
#define __KF_DEPLOY_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    // 命令回调函数
    typedef std::function<void( const std::string& param )> KFDeployFunction;

    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    class KFDeployClientInterface : public KFModule
    {
    public:
        template< class T >
        void RegisterFunction( const std::string& command, T* object, void( T::*handle )( const std::string& param ) )
        {
            KFDeployFunction function = std::bind( handle, object, std::placeholders::_1 );
            AddFunction( command, typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterFunction( const std::string& command, T* object )
        {
            RemoveFunction( command, typeid( T ).name() );
        }

    protected:
        virtual void AddFunction( const std::string& command, const std::string& module, KFDeployFunction& function ) = 0;
        virtual void RemoveFunction( const std::string& command, const std::string& module ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_deploy_client, KFDeployClientInterface );
    ///////////////////////////////////////////////////////////////////////
#define __KF_DEPLOY_FUNCTION__( function ) \
    void function( const std::string& param )

#define __REGISTER_DEPLOY_FUNCTION__( command, function ) \
    _kf_deploy_client->RegisterFunction( command, this, function )

#define __UN_DEPLOY_FUNCTION__( command ) \
    _kf_deploy_client->UnRegisterFunction( command, this )
}



#endif