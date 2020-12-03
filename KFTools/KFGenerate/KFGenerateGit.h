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

        // push更新
        virtual bool Push( const StringList& filelist, const std::string& message );

        // 拉取更新
        virtual bool Pull( bool commit, const std::string& message );

    private:
        // 仓库的会话
        git_repository* _git_repository = nullptr;
    };
}
#endif
