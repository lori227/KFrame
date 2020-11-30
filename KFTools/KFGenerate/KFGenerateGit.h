#ifndef __KF_GENERATE_GIT_H__
#define __KF_GENERATE_GIT_H__
#include "KFGenerateDefine.h"

namespace KFrame
{
    class KFGenerateGit : public KFSingleton< KFGenerateGit >
    {
    public:
        // 初始化
        void Initialize();

        // 关闭
        void Shutdown();

    private:

    };
    ///////////////////////////////////////////////////////////////////////////////////////////////
    static auto _git = KFrame::KFGenerateGit::Instance();
    ///////////////////////////////////////////////////////////////////////////////////////////////
}
#endif
