#ifndef __KF_LOADER_INTERFACE_H__
#define __KF_LOADER_INTERFACE_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFLoaderInterface : public KFModule
    {
    public:
        // 注册config
        template< class T >
        void AddConfig()
        {
            std::string name = typeid( T ).name();
            auto kfconfig = FindConfig( name );
            if ( kfconfig == nullptr )
            {
                kfconfig = __NEW_OBJECT__( T );
                AddConfig( name, kfconfig );
            }

            T::Initialize( dynamic_cast< T* >( kfconfig ) );
        }

        // 重新加载配置
        virtual void ReloadConfig() = 0;
    protected:
        // 创建config
        virtual void AddConfig(  const std::string& name, KFConfig* kfconfig ) = 0;

        // 查找config
        virtual KFConfig* FindConfig( const std::string& name ) = 0;
    };
    ///////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_loader, KFLoaderInterface );
    ///////////////////////////////////////////////////////////////////////
#define __KF_ADD_CONFIG__( name )  _kf_loader->AddConfig< name >()
}

#endif