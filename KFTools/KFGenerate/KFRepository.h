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

        // 添加文件
        virtual bool AddAllFile( const std::string& path )
        {
            return false;
        }

        // 提交修改
        virtual bool Commit( const std::string& message )
        {
            return false;
        }

        // 推送更新
        virtual bool Push()
        {
            return false;
        }

        // 拉取更新
        virtual bool Pull( bool commit, const std::string& message )
        {
            return false;
        }

    public:
        // 仓库配置
        const RepositoryData* _data = nullptr;
    };
}
#endif
