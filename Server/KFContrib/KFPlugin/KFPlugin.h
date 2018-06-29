#ifndef __KF_PLUGIN_H__
#define __KF_PLUGIN_H__

#include "KFMap.h"
#include "KFModule.h"

namespace KFrame
{
    class KFPluginManage;
    class KFPlugin : public KFInterface
    {
    public:

        KFPlugin();
        virtual ~KFPlugin();

        // 安装
        virtual void Install() = 0;

        // 卸载
        virtual void UnInstall() = 0;

        // 加载模块
        virtual void LoadModule() = 0;

        // 初始化
        virtual void InitModule();

        // 加载配置
        virtual void LoadConfig();
        virtual void AfterLoad();

        // 关闭
        virtual void BeforeShut();
        virtual void ShutDown();
        virtual void AfterShut();

        // 执行
        virtual void BeforeRun();
        virtual void OnceRun();
        //////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////

        // 注册模块
        template< class T >
        void RegistModule( T* module )
        {
            std::string name = typeid( T ).name();
            RegistModule( name, module );
        }

        // 卸载模块
        template< class T >
        void UnRegistModule()
        {
            std::string name = typeid( T ).name();
            UnRegistModule( name );
        }

        // 查找模块
        KFModule* FindModule( const std::string& name );

    protected:
        // 注册模块
        void RegistModule( const std::string& name, KFModule* module );

        // 卸载模块
        void UnRegistModule( const std::string& name );

        // 添加模块
        void AddModule( const std::string& name, KFModule* module );

        // 删除模块
        void RemoveModule( const std::string& name );

    public:
        // 排序
        uint32 _sort;

        // 配置文件
        std::string _config;

        // 模块列表
        std::map< std::string, KFModule* > _kf_module;
    };
}

#endif