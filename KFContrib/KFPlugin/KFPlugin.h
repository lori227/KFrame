#ifndef __KF_PLUGIN_H__
#define __KF_PLUGIN_H__

#include "KFMap.h"
#include "KFModule.h"

namespace KFrame
{
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

        // 添加配置
        virtual void AddConfig() {};

        // 初始化
        virtual void InitModule();

        // 加载配置
        virtual void LoadConfig();

        // 关闭
        virtual void BeforeShut();
        virtual void ShutDown();
        virtual void AfterShut();

        // 执行
        virtual void BeforeRun();
        virtual void PrepareRun();

        //////////////////////////////////////////////////////////////
        // 重新加载
        void Reload();
        //////////////////////////////////////////////////////////////
        // 注册模块
        void BindModule( const std::string& name, KFModule* module );

        // 卸载模块
        void UnBindModule( const std::string& name, bool savedata );

        // 查找模块
        KFModule* FindModule( const std::string& name );

    public:
        // 排序
        uint32 _sort;

        // 配置文件
        std::string _config;

        // 绑定模块
        std::map< std::string, KFModule* > _modules;

        // 是否需要保存数据
        bool _save_data = false;
    };

    //////////////////////////////////////////////////////////////////////
}

#endif