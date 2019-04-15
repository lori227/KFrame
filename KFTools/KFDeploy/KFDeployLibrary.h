#ifndef __KF_DEPLOY_LIBRARY_H__
#define __KF_DEPLOY_LIBRARY_H__

#include "KFrame.h"

namespace KFrame
{
    class KFDeployLibrary
    {
    public:
        KFDeployLibrary();
        ~KFDeployLibrary();

        // 加载插件
        bool Load( const std::string& path, const std::string& name );

        // 卸载插件
        void UnLoad();

        // 获得函数地址
        void* GetFunction( const char* function );

    protected:
        // 插件句柄
        void* _instance;

    public:
        // 插件路径
        std::string _path;
    };
}

#endif
