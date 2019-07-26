#ifndef __KF_CONFIG_INTERFACE_H__
#define __KF_CONFIG_INTERFACE_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFConfigInterface : public KFModule
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
                kfconfig = new T();
                AddConfig( name, kfconfig );
            }

            T::Initialize( dynamic_cast< T* >( kfconfig ) );
        }

        // 重新加载配置
        virtual void ReloadConfig( const std::string& file ) = 0;
    protected:
        // 创建config
        virtual void AddConfig(  const std::string& name, KFConfig* kfconfig ) = 0;

        // 查找config
        virtual KFConfig* FindConfig( const std::string& name ) = 0;
    };
    ///////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_config, KFConfigInterface );
    ///////////////////////////////////////////////////////////////////////
#define __KF_ADD_CONFIG__( name )  _kf_config->AddConfig< name >()
}

#endif