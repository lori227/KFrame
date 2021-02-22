#ifndef __KF_LIBRARY_H__
#define __KF_LIBRARY_H__

#include "KFrame.h"

namespace KFrame
{
    class KFLibrary
    {
    public:
        KFLibrary();
        ~KFLibrary();

        // 加载插件
        bool Load( const std::string& path, const std::string& name );

        // 卸载插件
        void UnLoad();

        // 查找函数
        void* GetFunction( const char* function );

    public:
        // 路径
        std::string _path;

    protected:
        // 句柄
        void* _instance;
    };
}

#endif
