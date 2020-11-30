#ifndef __KF_REPOSITORY_H__
#define __KF_REPOSITORY_H__
#include "KFGenerateDefine.h"

namespace KFrame
{
    class KFRepository
    {
    public:
        // 初始化
        virtual void Initialize() {};

        // 关闭
        virtual void Shutdown() {};

        // 打开
        virtual bool Open( const RepositoryData* data )
        {
            _data = data;
            return true;
        }

    public:
        // 仓库配置
        const RepositoryData* _data = nullptr;
    };
}
#endif
