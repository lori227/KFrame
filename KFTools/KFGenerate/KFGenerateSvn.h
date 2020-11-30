#ifndef __KF_GENERATE_SVN_H__
#define __KF_GENERATE_SVN_H__
#include "KFGenerateDefine.h"
#include "KFRepository.h"

struct git_repository;
namespace KFrame
{
    class KFGenerateSvn : public KFRepository
    {
    public:
        // 初始化
        virtual void Initialize();

        // 关闭
        virtual void Shutdown();

        // 打开仓库
        bool Open( const RepositoryData* data );
    private:
    };
}
#endif
