#include "KFGenerateGit.h"
#include "git2/global.h"

namespace KFrame
{
    void KFGenerateGit::Initialize()
    {
        git_libgit2_init();
    }

    void KFGenerateGit::Shutdown()
    {
        git_libgit2_shutdown();
    }
}
