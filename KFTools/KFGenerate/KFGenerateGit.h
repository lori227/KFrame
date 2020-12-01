#ifndef __KF_GENERATE_GIT_H__
#define __KF_GENERATE_GIT_H__
#include "KFGenerateDefine.h"
#include "KFRepository.h"

struct git_repository;
namespace KFrame
{
    class KFGenerateGit : public KFRepository
    {
    public:
        // 初始化
        virtual void Initialize();

        // 关闭
        virtual void Shutdown();

        // 打开仓库
        virtual bool Open( const RepositoryData* data );

        // 添加文件
        virtual bool AddAllFile( const std::string& path );

        // 提交修改
        virtual bool Commit( const std::string& message );

        // push更新
        virtual bool Push();
    private:
        // 仓库的会话
        git_repository* _git_repository = nullptr;
    };
}
#endif
