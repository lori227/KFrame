#include "KFGenerateGit.h"
#include "git2/global.h"
#include "git2/repository.h"
#include "git2/errors.h"
#include "git2/deprecated.h"
#include "KFGenerateEvent.h"

namespace KFrame
{
    void KFGenerateGit::Initialize()
    {
        git_libgit2_init();
        return KFRepository::Initialize();
    }

    void KFGenerateGit::Shutdown()
    {
        git_libgit2_shutdown();
        return KFRepository::Shutdown();
    }

    bool KFGenerateGit::Open( const RepositoryData* data )
    {
        auto result = git_repository_open( &_git_repository, data->_path.c_str() );
        if ( result < 0 )
        {
            auto error = giterr_last();
            _event->ShowEventMessage( __FORMAT__( "打开git仓库失败[{}:{}]", error->klass, error->message ) );
            return false;
        }

        return KFRepository::Open( data );
    }
}
